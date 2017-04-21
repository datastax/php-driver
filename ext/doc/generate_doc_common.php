<?php

define("INPUT_NAMESPACE", "Cassandra");

function logWarning($message) {
    fwrite(STDERR, "Warning: $message" . PHP_EOL);
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

function getClassNameWithNoCoreNamespace($className) {
    return preg_replace("/" . INPUT_NAMESPACE . "\\\\/", "", $className);
}

class YamlClassDoc {
    private $class;
    private $fileName;
    private $yamlFileName;
    private $doc;
    private static $classDocs = array();

    private function __construct($class, $fileName, $yamlFileName, $doc) {
        $this->class = $class;
        $this->fileName = $fileName;
        $this->yamlFileName = $yamlFileName;
        $this->doc = $doc;
    }

    public function getClass() {
        return $this->class;
    }

    public function getClassNameWithNoCoreNamespace() {
        return getClassNameWithNoCoreNamespace($this->class->getName());
    }

    public function getFileName() {
        return $this->fileName;
    }

    public function getYamlFileName() {
        return $this->yamlFileName;
    }

    public function getDoc() {
        return $this->doc;
    }

    public static function getClassDocs() {
        return self::$classDocs;
    }

    /**
     * Get the first non-empty class comment
     */
    public function getParentClassComment() {
        $classDocs = self::$classDocs;
        return self::getParentDocInternal($this->class, function($className) use($classDocs) {
            if ($className &&
                isset(self::$classDocs[$className]) &&
                !empty(trim(self::$classDocs[$className]->getDoc()["comment"]))) {
                return self::$classDocs[$className]->getDoc()["comment"];
            }
            return null;
        });
    }

    /**
     * Get the first non-empty method comment
     */
    public function getParentMethodComment($methodName) {
        $classDocs = self::$classDocs;
        return self::getParentDocInternal($this->class, function($className) use($classDocs, $methodName) {
            if ($className &&
                isset(self::$classDocs[$className]) &&
                isset(self::$classDocs[$className]->getDoc()["methods"][$methodName])) {
                $methodDoc = self::$classDocs[$className]->getDoc()["methods"][$methodName];
                if (!empty(trim($methodDoc["comment"]))) {
                    return $methodDoc["comment"];
                }
                return null;
            }
        });
    }

    /**
     * Get the first non-empty param doc
     */
    public function getParentParamDoc($methodName, $paramName) {
        $classDocs = self::$classDocs;
        return self::getParentDocInternal($this->class, function($className) use($classDocs, $methodName, $paramName) {
            if ($className &&
                isset(self::$classDocs[$className]) &&
                isset(self::$classDocs[$className]->getDoc()["methods"][$methodName]) &&
                isset(self::$classDocs[$className]->getDoc()["methods"][$methodName]['params'][$paramName])) {
                $paramDoc = self::$classDocs[$className]->getDoc()["methods"][$methodName]['params'][$paramName];
                if ($paramDoc["type"] != "mixed" && !empty(trim($paramDoc["comment"]))) {
                    return $paramDoc;
                }
            }
            return null;
        });
    }

    /**
     * Get the first non-empty return doc
     */
    public function getParentReturnDoc($methodName) {
        $classDocs = self::$classDocs;
        return self::getParentDocInternal($this->class, function($className) use($classDocs, $methodName) {
            if ($className &&
                isset(self::$classDocs[$className]) &&
                isset(self::$classDocs[$className]->getDoc()["methods"][$methodName])) {
                $methodDoc = self::$classDocs[$className]->getDoc()["methods"][$methodName];
                if ($methodDoc["return"]["type"] != "mixed" && !empty(trim($methodDoc["return"]["comment"]))) {
                    return $methodDoc["return"];
                }
            }
            return null;
        });
    }

    private static function getParentDocInternal($current, $getDoc) {
        $parent = $current->getParentClass();

        if ($parent) {
            $result = $getDoc($parent->getName());
            if ($result) {
                return $result;
            }
        }

        foreach ($current->getInterfaces() as $interface) {
            $result = $getDoc($interface->getName());
            if ($result) {
                return $result;
            }
        }

        if ($parent) {
            return self::getParentDocInternal($parent, $getDoc);
        }

        return null;
    }

    private static function loadYaml($class, $yamlFileName) {
        $doc = yaml_parse_file($yamlFileName);
        $classNameNoCoreNamespace = getClassNameWithNoCoreNamespace($class->getName());

        if ($doc === false) {
            $fullClassName = $class->getName();
            logWarning("Unable to load '$yamlFileName' for class '$fullClassName'");
            return array();
        } else if (isset($doc[$classNameNoCoreNamespace])){
            return $doc[$classNameNoCoreNamespace];
        } else {
            logWarning( "Unable to find top level class in '$yamlFileName'");
            return false;
        }

        return $doc;
    }

    private static function load($fileName, $dirName) {
        $yamlFileName = preg_replace("/(.+)\.c$/", "$1.yaml", $fileName);
        $fileName = substr($fileName, strlen($dirName));
        $fileName = preg_replace("/(.+)\.c$/", "$1", $fileName);

        if ($fileName == "/Core") {
            $fileName = "/" . INPUT_NAMESPACE;
            $fullClassName = str_replace("/", "\\", $fileName);
        } else {
            $fullClassName = INPUT_NAMESPACE . str_replace("/", "\\", $fileName);
        }

        try {
            $class = new ReflectionClass($fullClassName);
            $doc = self::loadYaml($class, $yamlFileName);
            if ($doc !== false) {
                self::$classDocs[$class->getName()] =
                    new YamlClassDoc($class, $fileName, $yamlFileName, $doc);
            }
        } catch(Exception $e) {
            logWarning("Ignoring '$fullClassName': $e");
        }
    }

    public static function loadAll($dirName) {
        $iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($dirName));
        $regex = new RegexIterator($iterator, '/^.+\.c$/i', RecursiveRegexIterator::GET_MATCH);

        foreach ($regex as $fileName => $notused) {
            echo "Loading... $fileName" . PHP_EOL;
            self::load($fileName, $dirName);
        }
    }
}
