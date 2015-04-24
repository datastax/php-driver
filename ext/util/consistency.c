#include "php_cassandra.h"

int php_cassandra_get_consistency(zval* consistency, long* result TSRMLS_DC)
{
  if (consistency && Z_TYPE_P(consistency) == IS_LONG) {
    switch (Z_LVAL_P(consistency)) {
    case CASS_CONSISTENCY_ANY:
    case CASS_CONSISTENCY_ONE:
    case CASS_CONSISTENCY_TWO:
    case CASS_CONSISTENCY_THREE:
    case CASS_CONSISTENCY_QUORUM:
    case CASS_CONSISTENCY_ALL:
    case CASS_CONSISTENCY_LOCAL_QUORUM:
    case CASS_CONSISTENCY_EACH_QUORUM:
    case CASS_CONSISTENCY_SERIAL:
    case CASS_CONSISTENCY_LOCAL_SERIAL:
    case CASS_CONSISTENCY_LOCAL_ONE:
      *result = Z_LVAL_P(consistency);
      break;
    default:
      INVALID_ARGUMENT_VALUE(consistency, "one of Cassandra::CONSISTENCY_*", FAILURE);
    }
  } else {
    INVALID_ARGUMENT_VALUE(consistency, "one of Cassandra::CONSISTENCY_*", FAILURE);
  }
  return SUCCESS;
}

int php_cassandra_get_serial_consistency(zval* serial_consistency, long* result TSRMLS_DC)
{
  if (serial_consistency && Z_TYPE_P(serial_consistency) == IS_LONG) {
    switch (Z_LVAL_P(serial_consistency)) {
    case CASS_CONSISTENCY_SERIAL:
    case CASS_CONSISTENCY_LOCAL_SERIAL:
      *result = Z_LVAL_P(serial_consistency);
      break;
    default:
      INVALID_ARGUMENT_VALUE(serial_consistency, "one of Cassandra::CONSISTENCY_SERIAL OR Cassanra::CASS_CONSISTENCY_LOCAL_SERIAL", FAILURE);
    }
  } else {
    INVALID_ARGUMENT_VALUE(serial_consistency, "one of Cassandra::CONSISTENCY_SERIAL OR Cassanra::CASS_CONSISTENCY_LOCAL_SERIAL", FAILURE);
  }
  return SUCCESS;
}
