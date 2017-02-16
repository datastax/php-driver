<?php

function logWarning($message) {
    fwrite(STDERR, "Warning: $message\n");
}

function doesParentHaveMethod($class, $method) {
    $parent = $class->getParentClass();
    if ($parent) {
        if ($parent->hasMethod($method->getName())) {
            return true;
        }
        return doesParentHaveMethod($parent, $method);
    }
    return false;
}

function writeDocYaml($yamlFileName, $class) {
    $classDoc = yaml_parse_file($yamlFileName);
    if ($classDoc === false) {
        $fullClassName = $class->getName();
        echo "Generating doc yaml file for '$fullClassName' ($yamlFileName)\n";
        $classDoc = array();
    } else if (isset($classDoc[$class->getName()])){
        $classDoc = $classDoc[$class->getName()];
    } else {
        logWarning( "Unable to find top level class in '$yamlFileName'\n");
        return;
    }
    if (!isset($classDoc["comment"])) {
        $classDoc["comment"] = "";
    } else {
        $classDoc["comment"] = $classDoc["comment"];
    }

    $constants = $class->getConstants();
    if ($constants) {
        $constantsDoc = isset($classDoc["constants"]) ? $classDoc["constants"] : array();
        foreach($constants as $name => $notused) {
            $constantDoc = isset($constantsDoc[$name]) ? $constantsDoc[$name] : array();
            if (!isset($constantDoc["comment"])) {
                $constantDoc["comment"] = "";
            } else {
                $constantDoc["comment"] = $constantDoc["comment"];
            }
            $constantsDoc[$name] = $constantDoc;
        }
        $classDoc["constants"] = $constantsDoc;
    }

    $methods = $class->getMethods(ReflectionMethod::IS_PUBLIC);
    if ($methods) {
        $methodsDoc = isset($classDoc["methods"]) ? $classDoc["methods"] : array();
        foreach($methods as $method) {
            if (doesParentHaveMethod($class, $method) && ($method->isStatic() || $method->isFinal())) {
                continue;
            }
            $methodDoc = isset($methodsDoc[$method->getShortName()]) ? $methodsDoc[$method->getShortName()] : array();
            if (!isset($methodDoc["comment"])) {
                $methodDoc["comment"] = "";
            } else {
                $methodDoc["comment"] = $methodDoc["comment"];
            }
            $parameters = $method->getParameters();
            if ($parameters) {
                $paramsDoc = isset($methodDoc["params"]) ? $methodDoc["params"] : array();
                foreach ($parameters as $parameter) {
                    $paramDoc = isset($paramsDoc[$parameter->getName()]) ? $paramsDoc[$parameter->getName()] : array();
                    if (!isset($paramDoc["comment"])) {
                        $paramDoc["comment"] = "";
                    } else {
                        $paramDoc["comment"] = $paramDoc["comment"];
                    }
                    if (!isset($paramDoc["type"])) {
                        $parameterType = $parameter->getType();
                        $paramDoc["type"] = $parameterType ? "$parameterType" : "mixed";
                    } else {
                        $paramDoc["type"] = $paramDoc["type"];
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
                    } else {
                        $returnDoc["comment"] = $returnDoc["comment"];
                    }
                    if (!isset($returnDoc["type"])) {
                        $returnDoc["type"] = "mixed";
                    } else {
                        $returnDoc["type"] = $returnDoc["type"];
                    }
                }
                $methodDoc["return"] = $returnDoc;
            }
            $methodsDoc[$method->getShortName()] = $methodDoc;
        }
        $classDoc["methods"] = $methodsDoc;
    }

    yaml_emit_file($yamlFileName, array($class->getName() => $classDoc));
}

function usage() {
    die("Usage: {$argv[0]} <directory>\n");
}

if (count($argv) < 1) {
    usage();
}

define("BASEDIR", $argv[1]);
define("SRCDIR", BASEDIR . "/src");

define("INPUT_NAMESPACE", "Cassandra");

$iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator(SRCDIR));
$regex = new RegexIterator($iterator, '/^.+\.c$/i', RecursiveRegexIterator::GET_MATCH);

foreach ($regex as $fileName => $notused) {
    $yamlFileName = preg_replace("/(.+)\.c$/", "$1.yaml", $fileName);
    $fileName = substr($fileName, strlen(SRCDIR));
    $fileName = preg_replace("/(.+)\.c$/", "$1", $fileName);

    if ($fileName == "/Core") {
        $fileName = "/" . INPUT_NAMESPACE;
        $fullClassName = str_replace("/", "\\", $fileName);
    } else {
        $fullClassName = INPUT_NAMESPACE . str_replace("/", "\\", $fileName);
    }

    try {
        $class = new ReflectionClass($fullClassName);

        writeDocYaml($yamlFileName, $class);
    } catch(ReflectionException $e) {
        logWarning("Ignoring '$fullClassName': $e");
    }
}
