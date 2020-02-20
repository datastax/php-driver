<?php

require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . "generate_doc_common.php");

define("LICENSE_COMMENT", "/**
 * Copyright 2017 DataStax, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the \"License\");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an \"AS IS\" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */");

define("DOC_COMMENT_HEADER", "/**" . PHP_EOL);
define("DOC_COMMENT_LINE", " * ");
define("DOC_COMMENT_LINE_EMPTY", " *");
define("DOC_COMMENT_FOOTER", " */" . PHP_EOL);

define("PHP_SYNTAX_CHECK", "php -l -n");

if (count($argv) < 2) {
    die("Usage: {$argv[0]} <directory>" . PHP_EOL);
}

define("BASEDIR", $argv[1]);
define("SRCDIR", BASEDIR . "/src");
define("DOCDIR", BASEDIR . "/doc");

define("OUTPUT_NAMESPACE", "Cassandra");
define("INDENT", "    ");

function startsWith($haystack, $needle) {
    $length = strlen($needle);
    return (substr($haystack, 0, $length) === $needle);
}

function isAlreadyImplementedByBase($current, $implemented) {
    $parentClass = $current->getParentClass();
    if ($parentClass && $parentClass != $implemented){
        if ($parentClass->implementsInterface($implemented) ||
            $parentClass->isSubclassOf($implemented)) {
                return true;
        }
    }

    $interfaces = $current->getInterfaces();
    foreach($interfaces as $interface) {
        if ($interface != $implemented) {
            if ($interface->implementsInterface($implemented) ||
                $interface->isSubclassOf($implemented)) {
                    return true;
                }
        }
    }

    return false;
}

function replaceKeyword($string) {
    if ($string == "Function") {
        return "Function_";
    } else if ($string == "function") {
        return "function_";
    } else if ($string == "Float") {
        return "Float_";
    }
    return $string;
}

function trimEmptyLines(&$lines) {
    while ($lines && strlen(trim($lines[0])) == 0) {
        array_pop($lines);
    }
    while ($lines && strlen(trim($lines[count($lines) - 1])) == 0) {
        array_pop($lines);
    }
}

function writeCommentLines($file, $lines, $indent) {
    foreach($lines as $line) {
        $commentLine = str_pad("", strlen(INDENT) * $indent, INDENT, STR_PAD_LEFT) . DOC_COMMENT_LINE . "$line";
        $commentLine = rtrim($commentLine) . PHP_EOL;
        fwrite($file, str_pad($commentLine, $indent, INDENT, STR_PAD_LEFT));
    }
}

function writeCommentDoc($file, $comment, $indent = 0) {
    $lines = explode(PHP_EOL, $comment);
    trimEmptyLines($lines);
    writeCommentLines($file, $lines, $indent);
}

function writeParamReturnCommentDoc($file, $typeAndName, $comment) {
    $lines = explode(PHP_EOL, $comment);
    if (strlen(end($lines)) == 0) {
        array_pop($lines);
    }

    $first = true;
    if ($lines) {
        foreach($lines as $line) {
            if ($first) {
                $commentLine = "$typeAndName $line";
                $commentLine = rtrim($commentLine) . PHP_EOL;
                fwrite($file, INDENT . DOC_COMMENT_LINE . $commentLine);
            } else {
                $commentLine = str_pad("", strlen($typeAndName) + 1, " ") . $line;
                $commentLine = rtrim($commentLine) . PHP_EOL;
                $docCommentLine = DOC_COMMENT_LINE;
                if ($commentLine == PHP_EOL) {
                    $docCommentLine = rtrim($docCommentLine);
                }
                fwrite($file, INDENT . $docCommentLine . $commentLine);
            }
            $first = false;
        }
    } else {
        fwrite($file, INDENT . DOC_COMMENT_LINE . $typeAndName . PHP_EOL);
    }
}

function writeParameterDoc($doc, $file, $class, $method, $parameter) {
    $className = $class->getName();
    $methodName = $method->getShortName();
    $parameterName = $parameter->getName();
    $parameterType = $parameter->getType();

    if (isset($doc[$className]['methods'][$methodName]['params'][$parameterName])) {
        $parameterDoc = $doc[$className]['methods'][$methodName]['params'][$parameterName];
        if (isset($parameterDoc['comment'])) {
            $comment = trim($parameterDoc['comment']);
        } else {
            logWarning("Missing '$parameterName' parameter's 'comment' documentation for method '$className::$methodName()'");
        }
        if (isset($parameterDoc['type'])) {
            $type = trim($parameterDoc['type']);
        } else {
            logWarning("Missing '$parameterName' parameter's 'type' documentation for method '$className::$methodName()'");
        }

        if ($parameterName == "...") {
            $parameterName = "params";
        }

        $parameterType = $parameterType ? $parameterType : "mixed";
        $parameterType = $type ? $type : $parameterType; # Overrides builtin if provided
        writeParamReturnCommentDoc($file, "@param $parameterType \$$parameterName", $comment);
    } else {
        $parameterType = $parameterType ? $parameterType : "mixed";
        fwrite($file, INDENT . DOC_COMMENT_LINE . "@param $parameterType \$$parameterName" . PHP_EOL);
        logWarning("Missing parameter '$parameterName' documentation for method '$className::$methodName()'");
    }
}

function writeReturnDoc($doc, $file, $class, $method) {
    $className = $class->getName();
    $methodName = $method->getShortName();

    if (isset($doc[$className]['methods'][$methodName]['return'])) {
        $returnDoc = $doc[$className]['methods'][$methodName]['return'];
        if (isset($returnDoc['comment'])) {
            $comment = trim($returnDoc['comment']);
        } else {
            logWarning("Missing return 'comment' documentation for method '$className::$methodName()'");
        }
        if (isset($returnDoc['type'])) {
            $type = trim($returnDoc['type']);
        } else {
            logWarning("Missing return 'type' documentation for method '$className::$methodName()'");
        }

        $type = $type ? $type : "mixed";
        writeParamReturnCommentDoc($file, "@return $type", $comment);
    } else {
        fwrite($file, INDENT . DOC_COMMENT_LINE . "@return mixed" . PHP_EOL);
        logWarning("Missing 'return' documentation for method '$className::$methodName()'");
    }
}

function writeConstantDoc($doc, $file, $class, $constantName) {
    $className = $class->getName();

    fwrite($file, INDENT . DOC_COMMENT_HEADER);
    if (isset($doc[$className]['constants'][$constantName])) {
        $constantDoc = $doc[$className]['constants'][$constantName];
        if (isset($constantDoc['comment'])) {
            $comment = $constantDoc['comment'];
        } else {
            logWarning("Missing 'comment' documentation for constant '$className::$constantName'");
        }

        if ($comment) {
            writeCommentDoc($file, $comment, 1);
        }
    } else {
        logWarning("Missing documentation for constant '$className::$constantName'");
    }
    fwrite($file, INDENT . DOC_COMMENT_FOOTER);
}

function writeConstant($doc, $file, $class, $constantName, $constantValue) {
    writeConstantDoc($doc, $file, $class, $constantName);

    if (is_int($constantValue)) {
        fwrite($file, INDENT . "const $constantName = $constantValue;" . PHP_EOL);
    } else {
        fwrite($file, INDENT . "const $constantName = '$constantValue';" . PHP_EOL);
    }
    fwrite($file, PHP_EOL);
}

function writeMethodCommentDoc($file, $comment, &$throws, &$sees) {
    $lines = array();

    $currentList = &$lines;
    foreach (explode(PHP_EOL, $comment) as $commentLine) {
        if (preg_match("/@throws/", $commentLine)) {
            $currentList = &$throws;
        } else if (preg_match("/@see/", $commentLine)) {
            $currentList = &$sees;
        } else if (strlen(trim($commentLine)) == 0) {
            $currentList = &$lines;
        }
        $currentList[] = $commentLine;
    }
    trimEmptyLines($lines);
    writeCommentLines($file, $lines, 1);
}

function writeMethodDoc($doc, $file, $class, $method) {
    if (!$method->isPublic()) return;

    $className = $class->getName();
    $methodName = $method->getShortName();

    fwrite($file, INDENT . DOC_COMMENT_HEADER);
    if (isset($doc[$className]['methods'][$methodName])) {
        $methodDoc = $doc[$className]['methods'][$methodName];
        if (isset($methodDoc['comment'])) {
            $comment = $methodDoc['comment'];
        } else {
            logWarning("Missing 'comment' documentation for method '$className::$methodName()'");
        }
        if (count($method->getParameters()) > 0 && !isset($methodDoc['params'])) {
            logWarning("Missing 'params' documentation for method '$className::$methodName()'");
        }

        $methodName = $method->getShortName();
        $parameters = $method->getParameters();

        $throws = array();
        $sees = array();
        $needsNewline = false;

        if ($comment) {
            writeMethodCommentDoc($file, $comment, $throws, $sees);
            $needsNewline = true;
        }

        if ($needsNewline && !empty($parameters)) {
            fwrite($file, INDENT . DOC_COMMENT_LINE_EMPTY . PHP_EOL);
            $needsNewline = true;
        }
        foreach ($parameters as $parameter) {
            writeParameterDoc($doc, $file, $class, $method, $parameter);
        }

        if ($needsNewline && !empty($throws)) {
            fwrite($file, INDENT . DOC_COMMENT_LINE_EMPTY . PHP_EOL);
            $needsNewline = true;
        }
        writeCommentLines($file, $throws, 1);

        if (!$method->isConstructor() && !$method->isDestructor()) {
            if ($needsNewline) {
                fwrite($file, INDENT . DOC_COMMENT_LINE_EMPTY . PHP_EOL);
                $needsNewline = true;
            }
            writeReturnDoc($doc, $file, $class, $method);
        }

        if ($needsNewline && !empty($sees))  {
            fwrite($file, INDENT . DOC_COMMENT_LINE_EMPTY . PHP_EOL);
        }
        writeCommentLines($file, $sees, 1);
    } else {
        fwrite($file, INDENT . DOC_COMMENT_LINE . "@return mixed" . PHP_EOL);
        logWarning("Missing documentation for method '$className::$methodName()'");
    }

    fwrite($file, INDENT . DOC_COMMENT_FOOTER);
}

function writeMethod($doc, $file, $class, $method) {
    if (doesParentHaveMethod($class, $method) &&
        ($method->isStatic() || $method->isFinal())) {
        return;
    }
    writeMethodDoc($doc, $file, $class, $method);

    if ($method->isPrivate()) {
        fwrite($file, INDENT . "private ");
    } else if ($method->isProtected()) {
        fwrite($file, INDENT . "protected ");
    } else if ($method->isPublic()) {
        fwrite($file, INDENT . "public ");
    }

    if ($method->isStatic()) {
        fwrite($file, "static ");
    }

    if ($method->isFinal()) {
        fwrite($file, "final ");
    }

    if (!$class->isInterface() && $method->isAbstract()) {
        fwrite($file, "abstract ");
    }

    $methodName = replaceKeyword($method->getShortName());
    fwrite($file, "function $methodName(");

    $parameters = $method->getParameters();
    $first = true;
    foreach ($parameters as $parameter) {
        if (!$first) fwrite($file, ", ");
        $parameterName = replaceKeyword($parameter->getName());
        if ($parameterName == "...") {
            fwrite($file, "...\$params");
        } else {
            fwrite($file, "\$$parameterName");
        }
        $first = false;
    }

    if ($class->isInterface() || $method->isAbstract()) {
        fwrite($file, ");" . PHP_EOL);
    } else {
        fwrite($file, ") { }" . PHP_EOL);
    }

    fwrite($file, PHP_EOL);
}

function writeClassDoc($doc, $file, $class) {
    $className = $class->getName();

    fwrite($file, DOC_COMMENT_HEADER);
    if (isset($doc[$className])) {
        if (isset($doc[$className]['comment'])) {
            writeCommentDoc($file, $doc[$className]['comment']);
        } else {
            logWarning("Missing 'comment' documentation for class '$className'");
        }
        if (count($class->getMethods(ReflectionMethod::IS_PUBLIC)) > 0 && !isset($doc[$className]['methods'])) {
            logWarning("Missing 'methods' documentation for class '$className'");
        }
    } else {
        fwrite($file, DOC_COMMENT_LINE . PHP_EOL);
        logWarning("Missing documentation for class '$className'");
    }
    fwrite($file, DOC_COMMENT_FOOTER);
}

function writeClass($doc, $file, $class) {
    $namespace = $class->getNamespaceName();
    $className = $class->getShortName();

    writeClassDoc($doc, $file, $class);

    if ($class->isInterface()) {
        fwrite($file, "interface ");
    } else if ($class->isAbstract()) {
        fwrite($file, "abstract class ");
    } else if ($class->isFinal()) {
        fwrite($file, "final class ");
    } else {
        fwrite($file, "class ");
    }

    $className = replaceKeyword($className);
    fwrite($file, "$className ");

    $parentClass = $class->getParentClass();
    if ($parentClass) {
        $parentClassName = $parentClass->getName();
        if (startsWith($parentClassName, $namespace) && $parentClassName !== $namespace) {
            $replacement = $namespace . '\\';
            $parentClassName = substr_replace($parentClassName, '', 0, strlen($replacement));
        } else {
            $parentClassName = "\\" . $parentClassName;
        }
        $parentClassName = replaceKeyword($parentClassName);
        fwrite($file, "extends $parentClassName ");
    }

    $interfaces = $class->getInterfaces();
    if ($interfaces) {
        $first = true;
        foreach ($interfaces as $interface) {
            if (isAlreadyImplementedByBase($class, $interface)) {
                continue;
            }
            $interfaceName = $interface->getName();
            if (startsWith($interfaceName, $namespace) && $interfaceName !== $namespace) {
                $replacement = $namespace . '\\';
                $interfaceName = substr_replace($interfaceName, '', 0, strlen($replacement));
            } else {
                $interfaceName = "\\" . $interfaceName;
            }

            if ($first) {
                fwrite($file, "implements ");
            } else {
                fwrite($file, ", ");
            }
            $interfaceName = replaceKeyword($interfaceName);
            fwrite($file, "$interfaceName");
            $first = false;
        }
        fwrite($file, " ");
    }

    fwrite($file, "{" . PHP_EOL);
    fwrite($file, PHP_EOL);

    $constants = $class->getConstants();
    if ($constants) {
        foreach($constants as $name => $value) {
            writeConstant($doc, $file, $class, $name, $value);
        }
    }

    $methods = $class->getMethods();
    if ($methods) {
        foreach($methods as $method) {
            writeMethod($doc, $file, $class, $method);
        }
    }

    fwrite($file, "}" . PHP_EOL);
}

function populateFromParent($classDoc, $doc) {
    if (empty(trim($doc["comment"]))) {
        $classComment = $classDoc->getParentClassComment();
        $doc["comment"] =  $classComment !== null ? $classComment : "";
    }
    if (isset($doc["methods"])) {
        foreach ($doc["methods"] as $methodName => &$methodDoc) {
            if (empty(trim($methodDoc["comment"]))) {
                $methodComment = $classDoc->getParentMethodComment($methodName);
                $methodDoc["comment"] = $methodComment !== null ? $methodComment : "";
            }

            if (isset($methodDoc["return"])) {
                $returnDoc = $methodDoc["return"];
                $parentReturnDoc = $classDoc->getParentReturnDoc($methodName);
                if ($parentReturnDoc !== null &&
                    empty(trim($returnDoc["comment"])) &&
                    ($returnDoc["type"] == "mixed" || $returnDoc["type"] == $parentReturnDoc["type"])) {
                        $methodDoc["return"]["type"] = $parentReturnDoc["type"];
                        $methodDoc["return"]["comment"] = $parentReturnDoc["comment"];
                    }
            }

            if (isset($methodDoc["params"])) {
                foreach ($methodDoc["params"] as $paramName => $paramDoc) {
                    $parentParamDoc = $classDoc->getParentParamDoc($methodName, $paramName);
                    if ($parentParamDoc !== null &&
                        empty(trim($paramDoc["comment"])) &&
                        ($paramDoc["type"] == "mixed" ||  $paramDoc["type"] == $parentParamDoc["type"])) {
                            $methodDoc["params"][$paramName]["type"] = $parentParamDoc["type"];
                            $methodDoc["params"][$paramName]["comment"] = $parentParamDoc["comment"];
                     }
                }
            }
        }
    }

    return $doc;
}

YamlClassDoc::loadAll(SRCDIR);

foreach(YamlClassDoc::getClassDocs() as $classDoc) {
    $fileName = $classDoc->getFileName();
    $yamlFileName = $classDoc->getYamlFileName();
    $class = $classDoc->getClass();
    $classNameNoCoreNamespace = $classDoc->getClassNameWithNoCoreNamespace();
    $doc = populateFromParent($classDoc, $classDoc->getDoc());

    if ($fileName == "/". INPUT_NAMESPACE) {
        $fullClassName = str_replace("/", "\\", $fileName);
        $namespaceDirectory = DOCDIR . "/" . dirname($fileName);
    } else {
        $fullClassName = INPUT_NAMESPACE . str_replace("/", "\\", $fileName);
        $namespaceDirectory = DOCDIR . "/" . OUTPUT_NAMESPACE . dirname($fileName);
    }

    if (!is_dir($namespaceDirectory)) {
        if (!mkdir($namespaceDirectory, 0777, true)) {
            die("Unable to create directory '$namespaceDirectory'" . PHP_EOL);
        }
    }

    $className = $class->getShortName();

    $stubFileName = rtrim($namespaceDirectory, "/")  . "/$className.php";
    echo "Generating stub for '$fullClassName' ($yamlFileName --> $stubFileName)" . PHP_EOL;
    if(!($file = fopen($stubFileName, "w"))) {
        die("Unable to create file '$stubFileName'" . PHP_EOL);
    }

    fwrite($file, "<?php" . PHP_EOL);
    fwrite($file, PHP_EOL);
    fwrite($file, LICENSE_COMMENT . PHP_EOL);
    fwrite($file, PHP_EOL);

    $namespace = $class->getNamespaceName();
    if ($namespace) {
        fwrite($file, "namespace $namespace;" . PHP_EOL);
        fwrite($file, PHP_EOL);
    }

    if ($classNameNoCoreNamespace === $class->getName()) {
        $doc = array($classNameNoCoreNamespace => $doc);
    } else {
        $doc = array($class->getName() => $doc);
    }

    writeClass($doc, $file, $class);

    fclose($file);

    exec(PHP_SYNTAX_CHECK . " $stubFileName", $syntaxCheckOutput, $syntaxCheckReturnVar);
    if ($syntaxCheckReturnVar !== 0) {
        $syntaxCheckOutput = implode(PHP_EOL, $syntaxCheckOutput);
        die("Syntax invalid for '$fullClassName' ($stubFileName): $syntaxCheckOutput" . PHP_EOL);
    }
}
