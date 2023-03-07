#pragma once

#include <php.h>

constexpr size_t ZendCPPClassPropertiesSize(const zend_class_entry *ce)
{
    return zend_object_properties_size((zend_class_entry *)ce);
}