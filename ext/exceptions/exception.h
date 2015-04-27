#ifndef PHP_CASSANDRA_EXCEPTION_H
#define PHP_CASSANDRA_EXCEPTION_H

// SPL should be enabled by default for PHP v5.3+
#if HAVE_SPL
# include <ext/spl/spl_exceptions.h>
#else
# error SPL must be enabled in order to build the driver
#endif

extern zend_class_entry* cassandra_ce_Exception;
extern zend_class_entry* cassandra_ce_RuntimeException;
extern zend_class_entry* cassandra_ce_InvalidArgumentException;

#endif /* PHP_CASSANDRA_EXCEPTION_H */
