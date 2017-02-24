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
    yaml_emit_file($yamlFileName, array($this->classNameNoCoreNamespace => $classDoc));
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

class YamlClassDoc {
    private $class;
    private $yamlFileName;
    private $classDoc;
    private static $classDocs = array();

    public function __construct($class, $yamlFileName, $classDoc) {
        $this->class = $class;
        $this->yamlFileName = $yamlFileName;
        $this->classDoc = $classDoc;
        self::$classDocs[$this->class->getName()] = $this;
    }


    public function emit() {
        $classNameNoCoreNamespace = self::getClassNameWithNoCoreNamespace($this->class->getName());
        yaml_emit_file($this->yamlFileName, array($classNameNoCoreNamespace => $this->classDoc));
    }

    public static function emitAll() {
        foreach (self::$classDocs as $classDoc) {
            $classDoc->emit();
        }
    }

    public function fixTypes() {
        if (isset($this->classDoc["methods"])) {
            foreach ($this->classDoc["methods"] as $methodName => $methodDoc) {
                $parentMethodDoc = $this->getParentMethodDoc($methodName);
                if ($parentMethodDoc) {
                    if (empty($methodDoc["comment"]) && !empty($parentMethodDoc["comment"])) {
                        $this->classDoc["methods"][$methodName]["comment"] = "{@inheritDoc}";
                    }
                    if (isset($methodDoc["return"])) {
                        $returnDoc = $methodDoc["return"];
                        $parentReturnDoc = $parentMethodDoc["return"];
                        if ($returnDoc["type"] == "mixed" && (empty($returnDoc["comment"]) || $returnDoc["comment"] == "{@inheritDoc}")) {
                            $this->classDoc["methods"][$methodName]["return"]["type"] = $parentReturnDoc["type"];
                            if (empty($returnDoc["comment"]) && !empty($parentReturnDoc["comment"])) {
                                $this->classDoc["methods"][$methodName]["return"]["comment"] = "{@inheritDoc}";
                            }
                        }
                    }
                    if (isset($methodDoc["params"])) {
                        $parentParamDocs = $parentMethodDoc["params"];
                        foreach ($methodDoc["params"] as $paramName => $paramDoc) {
                            if (isset($parentParamDocs[$paramName])) {
                                if ($paramDoc["type"] == "mixed" && (empty($paramDoc["comment"]) || $paramDoc["comment"] == "{@inheritDoc}")) {
                                    $this->classDoc["methods"][$methodName]["params"][$paramName]["type"] = $parentParamDocs[$paramName]["type"];
                                    if (empty($paramDoc["comment"]) && !empty($parentParamDocs[$paramName]["comment"])) {
                                        $this->classDoc["methods"][$methodName]["params"][$paramName]["comment"] = "{@inheritDoc}";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    public static function fixAllTypes() {
        foreach (self::$classDocs as $classDoc) {
            $classDoc->fixTypes();
        }
    }

    public function getParentMethodDoc($methodName) {
        $parent = $this->class->getParentClass();
        $className = null;
        if ($parent && $parent->hasMethod($methodName)) {
            $className = $parent->getName();
        } else {
            foreach ($this->class->getInterfaces() as $interface) {
                if ($interface->hasMethod($methodName)) {
                    $className = $interface->getName();
                    break;
                }
            }
        }

        if ($className && isset(self::$classDocs[$className])) {
            return self::$classDocs[$className]->classDoc["methods"][$methodName];
        }

        return null;
    }

    private static function getClassNameWithNoCoreNamespace($className) {
        return preg_replace("/" . INPUT_NAMESPACE . "\\\\/", "", $className);
    }

    private static function loadYaml($class, $yamlFileName) {
        $classDoc = yaml_parse_file($yamlFileName);
        $classNameNoCoreNamespace = self::getClassNameWithNoCoreNamespace($class->getName());
        print("$classNameNoCoreNamespace\n");

        if ($classDoc === false) {
            $fullClassName = $class->getName();
            echo "Generating doc yaml file for '$fullClassName' ($yamlFileName)\n";
            $classDoc = array();
        } else if (isset($classDoc[$classNameNoCoreNamespace])){
            $classDoc = $classDoc[$classNameNoCoreNamespace];
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

        return $classDoc;
    }

    public static function load($fileName) {
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
            $classDoc = self::loadYaml($class, $yamlFileName);

            return new YamlClassDoc($class, $yamlFileName, $classDoc);
            #writeDocYaml($yamlFileName, $class);
        } catch(ReflectionException $e) {
            logWarning("Ignoring '$fullClassName': $e");
        }
    }
};

foreach ($regex as $fileName => $notused) {
    print "Loading... $fileName" . PHP_EOL;
    YamlClassDoc::load($fileName);
}

YamlClassDoc::fixAllTypes();
YamlClassDoc::emitAll();
