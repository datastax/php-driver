<?php

require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . "generate_doc_common.php");

function usage() {
    die("Usage: {$argv[0]} <directory>\n");
}

if (count($argv) < 1) {
    usage();
}

define("BASEDIR", $argv[1]);
define("SRCDIR", BASEDIR . "/src");

/**
 * Fix types, remove '{@inheritDoc}' and remove redundant comments (the same as
 * the parent)
 */
function fixTypes($classDoc, $doc) {
    if (trim($doc["comment"]) == "{@inheritDoc}") {
        $doc["comment"] = "";
    }
    if (isset($doc["methods"])) {
        foreach ($doc["methods"] as $methodName => &$methodDoc) {

            if (trim($methodDoc["comment"]) == "{@inheritDoc}") {
                $methodDoc["comment"] = "";
            } else {
                $methodComment = $classDoc->getParentMethodComment($methodName);
                if ($methodComment !== null && trim($methodDoc["comment"]) == trim($methodComment)) {
                    $methodDoc["comment"] = "";
                }
            }

            if (isset($methodDoc["return"])) {
                $returnDoc = $methodDoc["return"];

                if (trim($returnDoc["comment"]) == "{@inheritDoc}") {
                    $methodDoc["return"]["comment"] = "";
                }

                $parentReturnDoc = $classDoc->getParentReturnDoc($methodName);
                if ($parentReturnDoc !== null) {
                    if ($returnDoc["type"] == "mixed" && empty($returnDoc["comment"])) {
                        $methodDoc["return"]["type"] = $parentReturnDoc["type"];
                        $methodDoc["return"]["comment"] = "";
                    } else if (trim($returnDoc["comment"]) == trim($parentReturnDoc["comment"])) {
                        $methodDoc["return"]["comment"] = "";
                    }
                }
            }

            if (isset($methodDoc["params"])) {
                foreach ($methodDoc["params"] as $paramName => $paramDoc) {
                    if (trim($paramDoc["comment"]) == "{@inheritDoc}") {
                        $methodDoc["params"][$paramName]["comment"] = "";
                    }

                    $parentParamDoc = $classDoc->getParentParamDoc($methodName, $paramName);
                    if ($parentParamDoc !== null) {
                        if ($paramDoc["type"] == "mixed" && empty($paramDoc["comment"])) {
                            $methodDoc["params"][$paramName]["type"] = $parentParamDoc["type"];
                            $methodDoc["params"][$paramName]["comment"] = "";
                        } else if (trim($paramDoc["comment"]) == trim($parentParamDoc["comment"])) {
                            $methodDoc["params"][$paramName]["comment"] = "";
                        }
                    }
                }
            }
        }
    }

    return $doc;
}

/**
 * Fill in a skeleton for types that have an implemention (a .c file), but
 * no .yaml file
 */
function populateEmptyDocs($classDoc, $doc) {
    $class = $classDoc->getClass();

    if (!isset($doc["comment"])) {
        $doc["comment"] = "";
    }

    $constants = $class->getConstants();
    if ($constants) {
        $constantsDoc = isset($doc["constants"]) ? $doc["constants"] : array();
        foreach($constants as $name => $notused) {
            $constantDoc = isset($constantsDoc[$name]) ? $constantsDoc[$name] : array();
            if (!isset($constantDoc["comment"])) {
                $constantDoc["comment"] = "";
            }
            $constantsDoc[$name] = $constantDoc;
        }
        $doc["constants"] = $constantsDoc;
    }

    $methods = $class->getMethods(ReflectionMethod::IS_PUBLIC);
    if ($methods) {
        $methodsDoc = isset($doc["methods"]) ? $doc["methods"] : array();
        foreach($methods as $method) {
            if (doesParentHaveMethod($class, $method) && ($method->isStatic() || $method->isFinal())) {
                continue;
            }
            $methodDoc = isset($methodsDoc[$method->getShortName()]) ? $methodsDoc[$method->getShortName()] : array();
            if (!isset($methodDoc["comment"])) {
                $methodDoc["comment"] = "";
            }
            $parameters = $method->getParameters();
            if ($parameters) {
                $paramsDoc = isset($methodDoc["params"]) ? $methodDoc["params"] : array();
                foreach ($parameters as $parameter) {
                    $paramDoc = isset($paramsDoc[$parameter->getName()]) ? $paramsDoc[$parameter->getName()] : array();
                    if (!isset($paramDoc["comment"])) {
                        $paramDoc["comment"] = "";
                    }
                    if (!isset($paramDoc["type"])) {
                        $parameterType = $parameter->getType();
                        $paramDoc["type"] = $parameterType ? "$parameterType" : "mixed";
                    }
                    $paramsDoc[$parameter->getName()] = $paramDoc;
                }
                $methodDoc["params"] = $paramsDoc;
            }
            if (!$method->isConstructor() && !$method->isDestructor()) {
                if (!isset($methodDoc["return"])) {
                    $returnDoc = array();
                    $returnDoc["comment"] = "";
                    $returnDoc["type"] = "mixed";
                } else {
                    $returnDoc = $methodDoc["return"];
                    if (!isset($returnDoc["comment"])) {
                        $returnDoc["comment"] = "";
                    }
                    if (!isset($returnDoc["type"])) {
                        $returnDoc["type"] = "mixed";
                    }
                }
                $methodDoc["return"] = $returnDoc;
            }
            $methodsDoc[$method->getShortName()] = $methodDoc;
        }
        $doc["methods"] = $methodsDoc;
    }

    return $doc;
}

YamlClassDoc::loadAll(SRCDIR);

foreach(YamlClassDoc::getClassDocs() as $classDoc) {
    $doc = populateEmptyDocs($classDoc, $classDoc->getDoc());
    $doc = fixTypes($classDoc, $doc);
    yaml_emit_file($classDoc->getYamlFileName(), array($classDoc->getClassNameWithNoCoreNamespace() => $doc));
}
