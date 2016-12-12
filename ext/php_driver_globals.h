#ifndef PHP_DRIVER_GLOBALS_H
#define PHP_DRIVER_GLOBALS_H

ZEND_BEGIN_MODULE_GLOBALS(php_driver)
  CassUuidGen  *uuid_gen;
  pid_t         uuid_gen_pid;
  unsigned int  persistent_clusters;
  unsigned int  persistent_sessions;
  php5to7_zval  type_varchar;
  php5to7_zval  type_text;
  php5to7_zval  type_blob;
  php5to7_zval  type_ascii;
  php5to7_zval  type_bigint;
  php5to7_zval  type_counter;
  php5to7_zval  type_int;
  php5to7_zval  type_varint;
  php5to7_zval  type_boolean;
  php5to7_zval  type_decimal;
  php5to7_zval  type_double;
  php5to7_zval  type_float;
  php5to7_zval  type_inet;
  php5to7_zval  type_timestamp;
  php5to7_zval  type_date;
  php5to7_zval  type_time;
  php5to7_zval  type_uuid;
  php5to7_zval  type_timeuuid;
  php5to7_zval  type_smallint;
  php5to7_zval  type_tinyint;
ZEND_END_MODULE_GLOBALS(php_driver)

ZEND_EXTERN_MODULE_GLOBALS(php_driver)

#endif /* PHP_DRIVER_GLOBALS_H */
