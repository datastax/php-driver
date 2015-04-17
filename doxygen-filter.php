#!/usr/bin/env php
<?php

// Input
$source = file_get_contents($argv[1]);
$tokens = token_get_all($source);
$buffer = '';
foreach ($tokens as $token) {
    if (is_string($token)) {
        if (!empty($buffer) && $token == ';') {
            echo $buffer;
            $buffer = '';
        }
        echo $token;
    } else {
        list($id, $text) = $token;
        switch ($id) {
        case T_DOC_COMMENT:
            // replace @return with @retval
            $text  = preg_replace('#@return\s#', '@retval ', $text);
            // replace starting namespace separator
            $text  = preg_replace('#(\s)\\\\([A-Z]\w+)#ms', '$1$2', $text);
            $count = 0;
            do {
                // replace backslash in comment
                $text = preg_replace('#(\*\s*[^*]*?\b\w+[^\n\r ]+)\\\\([A-Z])#ms', '$1::$2', $text, 1, $count);
            } while ($count);
            // optimize @var tags
            if (preg_match('#@var\s+[^\$]*\*/#ms', $text)) {
                $buffer = preg_replace('#(@var\s+[^\n\r]+)(\n\r?.*\*/)#ms',
                    '$1 \$\$\$$2', $text);
            } else {
                echo $text;
            }
            break;

        case T_VARIABLE:
            if (!empty($buffer)) {
                echo str_replace('$$$', $text, $buffer);
                unset($buffer);
            }
            echo $text;
            break;

        case T_NS_SEPARATOR:
            if (!empty($buffer)) {
                $buffer .= '::';
            } else {
                echo '::';
            }
            break;
        default:
            if (!empty($buffer)) {
                $buffer .= $text;
            } else {
                echo $text;
            }
            break;
        }
    }
}
if (!empty($buffer)) {
    echo $buffer;
}
