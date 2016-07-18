if (!String.prototype.trim) {
  (function() {
    // Make sure we trim BOM and NBSP
    var rtrim = /^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g;
    String.prototype.trim = function() {
        return this.replace(rtrim, '');
    };
  })();
}

(function(window) {
  function basePath() {
    var regexp = new RegExp('js/app.js');
    var script = $('script').filter(function(i, el) {
      return el.src.match(regexp);
    })[0]

    var base = script.src.substr(window.location.protocol.length + window.location.host.length + 2, script.src.length);

    return base.replace('/js/app.js', '');
  }

  var app = window.angular.module('docs', ['cfp.hotkeys'])

  app.value('pages', {"/":{"title":"Home","summary":"Home <small class=\"text-muted\">page</small>","path":"/","version":"local"},"/features/":{"title":"Features","summary":"Features <small class=\"text-muted\">page</small>","path":"/features/","version":"local"},"/features/sessions/":{"title":"Sessions","summary":"Sessions <small class=\"text-muted\">page</small>","path":"/features/sessions/","version":"local"},"/features/batch_statements/":{"title":"Batch statements","summary":"Batch statements <small class=\"text-muted\">feature</small>","path":"/features/batch_statements/","version":"local"},"/features/client_side_timestamps/":{"title":"Client-side timestamps","summary":"Client-side timestamps <small class=\"text-muted\">feature</small>","path":"/features/client_side_timestamps/","version":"local"},"/features/collections/":{"title":"Collections","summary":"Collections <small class=\"text-muted\">feature</small>","path":"/features/collections/","version":"local"},"/features/consistency_level/":{"title":"Consistency Level","summary":"Consistency Level <small class=\"text-muted\">feature</small>","path":"/features/consistency_level/","version":"local"},"/features/datatypes/":{"title":"Datatypes","summary":"Datatypes <small class=\"text-muted\">feature</small>","path":"/features/datatypes/","version":"local"},"/features/function_and_aggregate_metadata/":{"title":"User-defined Function (UDF) and Aggregate Metadata (UDA)","summary":"User-defined Function (UDF) and Aggregate Metadata (UDA) <small class=\"text-muted\">feature</small>","path":"/features/function_and_aggregate_metadata/","version":"local"},"/features/logging/":{"title":"Logging","summary":"Logging <small class=\"text-muted\">feature</small>","path":"/features/logging/","version":"local"},"/features/materialized_view_metadata/":{"title":"Materialized View Metadata","summary":"Materialized View Metadata <small class=\"text-muted\">feature</small>","path":"/features/materialized_view_metadata/","version":"local"},"/features/prepared_statements/":{"title":"Prepared Statements","summary":"Prepared Statements <small class=\"text-muted\">feature</small>","path":"/features/prepared_statements/","version":"local"},"/features/result_paging/":{"title":"Result paging","summary":"Result paging <small class=\"text-muted\">feature</small>","path":"/features/result_paging/","version":"local"},"/features/retry_polices/":{"title":"Retry Policies","summary":"Retry Policies <small class=\"text-muted\">feature</small>","path":"/features/retry_polices/","version":"local"},"/features/schema_metadata/":{"title":"Schema Metadata","summary":"Schema Metadata <small class=\"text-muted\">feature</small>","path":"/features/schema_metadata/","version":"local"},"/features/secondary_index_metadata/":{"title":"Secondary Index Metadata","summary":"Secondary Index Metadata <small class=\"text-muted\">feature</small>","path":"/features/secondary_index_metadata/","version":"local"},"/features/sessions/persistent_sessions/":{"title":"Persistent Sessions","summary":"Persistent Sessions <small class=\"text-muted\">feature</small>","path":"/features/sessions/persistent_sessions/","version":"local"},"/features/sessions/session_management/":{"title":"Session management","summary":"Session management <small class=\"text-muted\">feature</small>","path":"/features/sessions/session_management/","version":"local"},"/features/sessions/session_object/":{"title":"Session Object","summary":"Session Object <small class=\"text-muted\">feature</small>","path":"/features/sessions/session_object/","version":"local"},"/features/simple_statements/":{"title":"Simple Statements","summary":"Simple Statements <small class=\"text-muted\">feature</small>","path":"/features/simple_statements/","version":"local"},"/features/ssl_encryption/":{"title":"SSL encryption","summary":"SSL encryption <small class=\"text-muted\">feature</small>","path":"/features/ssl_encryption/","version":"local"},"/features/tuples/":{"title":"Tuples","summary":"Tuples <small class=\"text-muted\">feature</small>","path":"/features/tuples/","version":"local"},"/features/user_defined_types/":{"title":"User defined types","summary":"User defined types <small class=\"text-muted\">feature</small>","path":"/features/user_defined_types/","version":"local"},"/api/class.Cassandra/":{"title":"Cassandra","summary":"Cassandra <small class=\"text-muted\">class</small>","path":"/api/class.Cassandra/","version":"local"},"/api/Cassandra/class.BatchStatement/":{"title":"BatchStatement","summary":"BatchStatement <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.BatchStatement/","version":"local"},"/api/Cassandra/class.Bigint/":{"title":"Bigint","summary":"Bigint <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Bigint/","version":"local"},"/api/Cassandra/class.Blob/":{"title":"Blob","summary":"Blob <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Blob/","version":"local"},"/api/Cassandra/Cluster/class.Builder/":{"title":"Builder","summary":"Builder <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Cluster/class.Builder/","version":"local"},"/api/Cassandra/class.Collection/":{"title":"Collection","summary":"Collection <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Collection/","version":"local"},"/api/Cassandra/class.Date/":{"title":"Date","summary":"Date <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Date/","version":"local"},"/api/Cassandra/class.Decimal/":{"title":"Decimal","summary":"Decimal <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Decimal/","version":"local"},"/api/Cassandra/class.DefaultAggregate/":{"title":"DefaultAggregate","summary":"DefaultAggregate <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultAggregate/","version":"local"},"/api/Cassandra/class.DefaultCluster/":{"title":"DefaultCluster","summary":"DefaultCluster <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultCluster/","version":"local"},"/api/Cassandra/class.DefaultColumn/":{"title":"DefaultColumn","summary":"DefaultColumn <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultColumn/","version":"local"},"/api/Cassandra/class.DefaultIndex/":{"title":"DefaultIndex","summary":"DefaultIndex <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultIndex/","version":"local"},"/api/Cassandra/class.DefaultKeyspace/":{"title":"DefaultKeyspace","summary":"DefaultKeyspace <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultKeyspace/","version":"local"},"/api/Cassandra/class.DefaultMaterializedView/":{"title":"DefaultMaterializedView","summary":"DefaultMaterializedView <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultMaterializedView/","version":"local"},"/api/Cassandra/class.DefaultSchema/":{"title":"DefaultSchema","summary":"DefaultSchema <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultSchema/","version":"local"},"/api/Cassandra/class.DefaultSession/":{"title":"DefaultSession","summary":"DefaultSession <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultSession/","version":"local"},"/api/Cassandra/class.DefaultTable/":{"title":"DefaultTable","summary":"DefaultTable <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.DefaultTable/","version":"local"},"/api/Cassandra/Exception/class.AlreadyExistsException/":{"title":"AlreadyExistsException","summary":"AlreadyExistsException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.AlreadyExistsException/","version":"local"},"/api/Cassandra/Exception/class.AuthenticationException/":{"title":"AuthenticationException","summary":"AuthenticationException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.AuthenticationException/","version":"local"},"/api/Cassandra/Exception/class.ConfigurationException/":{"title":"ConfigurationException","summary":"ConfigurationException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.ConfigurationException/","version":"local"},"/api/Cassandra/Exception/class.DivideByZeroException/":{"title":"DivideByZeroException","summary":"DivideByZeroException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.DivideByZeroException/","version":"local"},"/api/Cassandra/Exception/class.DomainException/":{"title":"DomainException","summary":"DomainException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.DomainException/","version":"local"},"/api/Cassandra/Exception/class.ExecutionException/":{"title":"ExecutionException","summary":"ExecutionException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.ExecutionException/","version":"local"},"/api/Cassandra/Exception/class.InvalidArgumentException/":{"title":"InvalidArgumentException","summary":"InvalidArgumentException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.InvalidArgumentException/","version":"local"},"/api/Cassandra/Exception/class.InvalidQueryException/":{"title":"InvalidQueryException","summary":"InvalidQueryException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.InvalidQueryException/","version":"local"},"/api/Cassandra/Exception/class.InvalidSyntaxException/":{"title":"InvalidSyntaxException","summary":"InvalidSyntaxException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.InvalidSyntaxException/","version":"local"},"/api/Cassandra/Exception/class.IsBootstrappingException/":{"title":"IsBootstrappingException","summary":"IsBootstrappingException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.IsBootstrappingException/","version":"local"},"/api/Cassandra/Exception/class.LogicException/":{"title":"LogicException","summary":"LogicException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.LogicException/","version":"local"},"/api/Cassandra/Exception/class.OverloadedException/":{"title":"OverloadedException","summary":"OverloadedException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.OverloadedException/","version":"local"},"/api/Cassandra/Exception/class.ProtocolException/":{"title":"ProtocolException","summary":"ProtocolException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.ProtocolException/","version":"local"},"/api/Cassandra/Exception/class.RangeException/":{"title":"RangeException","summary":"RangeException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.RangeException/","version":"local"},"/api/Cassandra/Exception/class.ReadTimeoutException/":{"title":"ReadTimeoutException","summary":"ReadTimeoutException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.ReadTimeoutException/","version":"local"},"/api/Cassandra/Exception/class.RuntimeException/":{"title":"RuntimeException","summary":"RuntimeException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.RuntimeException/","version":"local"},"/api/Cassandra/Exception/class.ServerException/":{"title":"ServerException","summary":"ServerException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.ServerException/","version":"local"},"/api/Cassandra/Exception/class.TimeoutException/":{"title":"TimeoutException","summary":"TimeoutException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.TimeoutException/","version":"local"},"/api/Cassandra/Exception/class.TruncateException/":{"title":"TruncateException","summary":"TruncateException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.TruncateException/","version":"local"},"/api/Cassandra/Exception/class.UnauthorizedException/":{"title":"UnauthorizedException","summary":"UnauthorizedException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.UnauthorizedException/","version":"local"},"/api/Cassandra/Exception/class.UnavailableException/":{"title":"UnavailableException","summary":"UnavailableException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.UnavailableException/","version":"local"},"/api/Cassandra/Exception/class.UnpreparedException/":{"title":"UnpreparedException","summary":"UnpreparedException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.UnpreparedException/","version":"local"},"/api/Cassandra/Exception/class.ValidationException/":{"title":"ValidationException","summary":"ValidationException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.ValidationException/","version":"local"},"/api/Cassandra/Exception/class.WriteTimeoutException/":{"title":"WriteTimeoutException","summary":"WriteTimeoutException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Exception/class.WriteTimeoutException/","version":"local"},"/api/Cassandra/class.ExecutionOptions/":{"title":"ExecutionOptions","summary":"ExecutionOptions <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.ExecutionOptions/","version":"local"},"/api/Cassandra/class.Float/":{"title":"Float","summary":"Float <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Float/","version":"local"},"/api/Cassandra/class.FutureClose/":{"title":"FutureClose","summary":"FutureClose <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.FutureClose/","version":"local"},"/api/Cassandra/class.FutureException/":{"title":"FutureException","summary":"FutureException <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.FutureException/","version":"local"},"/api/Cassandra/class.FuturePreparedStatement/":{"title":"FuturePreparedStatement","summary":"FuturePreparedStatement <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.FuturePreparedStatement/","version":"local"},"/api/Cassandra/class.FutureRows/":{"title":"FutureRows","summary":"FutureRows <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.FutureRows/","version":"local"},"/api/Cassandra/class.FutureSession/":{"title":"FutureSession","summary":"FutureSession <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.FutureSession/","version":"local"},"/api/Cassandra/class.FutureValue/":{"title":"FutureValue","summary":"FutureValue <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.FutureValue/","version":"local"},"/api/Cassandra/class.Inet/":{"title":"Inet","summary":"Inet <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Inet/","version":"local"},"/api/Cassandra/class.Map/":{"title":"Map","summary":"Map <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Map/","version":"local"},"/api/Cassandra/class.PreparedStatement/":{"title":"PreparedStatement","summary":"PreparedStatement <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.PreparedStatement/","version":"local"},"/api/Cassandra/RetryPolicy/class.DefaultPolicy/":{"title":"DefaultPolicy","summary":"DefaultPolicy <small class=\"text-muted\">class</small>","path":"/api/Cassandra/RetryPolicy/class.DefaultPolicy/","version":"local"},"/api/Cassandra/RetryPolicy/class.DowngradingConsistency/":{"title":"DowngradingConsistency","summary":"DowngradingConsistency <small class=\"text-muted\">class</small>","path":"/api/Cassandra/RetryPolicy/class.DowngradingConsistency/","version":"local"},"/api/Cassandra/RetryPolicy/class.Fallthrough/":{"title":"Fallthrough","summary":"Fallthrough <small class=\"text-muted\">class</small>","path":"/api/Cassandra/RetryPolicy/class.Fallthrough/","version":"local"},"/api/Cassandra/RetryPolicy/class.Logging/":{"title":"Logging","summary":"Logging <small class=\"text-muted\">class</small>","path":"/api/Cassandra/RetryPolicy/class.Logging/","version":"local"},"/api/Cassandra/class.Rows/":{"title":"Rows","summary":"Rows <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Rows/","version":"local"},"/api/Cassandra/class.SSLOptions/":{"title":"SSLOptions","summary":"SSLOptions <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.SSLOptions/","version":"local"},"/api/Cassandra/SSLOptions/class.Builder/":{"title":"Builder","summary":"Builder <small class=\"text-muted\">class</small>","path":"/api/Cassandra/SSLOptions/class.Builder/","version":"local"},"/api/Cassandra/class.Set/":{"title":"Set","summary":"Set <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Set/","version":"local"},"/api/Cassandra/class.SimpleStatement/":{"title":"SimpleStatement","summary":"SimpleStatement <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.SimpleStatement/","version":"local"},"/api/Cassandra/class.Smallint/":{"title":"Smallint","summary":"Smallint <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Smallint/","version":"local"},"/api/Cassandra/class.Time/":{"title":"Time","summary":"Time <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Time/","version":"local"},"/api/Cassandra/class.Timestamp/":{"title":"Timestamp","summary":"Timestamp <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Timestamp/","version":"local"},"/api/Cassandra/TimestampGenerator/class.Monotonic/":{"title":"Monotonic","summary":"Monotonic <small class=\"text-muted\">class</small>","path":"/api/Cassandra/TimestampGenerator/class.Monotonic/","version":"local"},"/api/Cassandra/TimestampGenerator/class.ServerSide/":{"title":"ServerSide","summary":"ServerSide <small class=\"text-muted\">class</small>","path":"/api/Cassandra/TimestampGenerator/class.ServerSide/","version":"local"},"/api/Cassandra/class.Timeuuid/":{"title":"Timeuuid","summary":"Timeuuid <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Timeuuid/","version":"local"},"/api/Cassandra/class.Tinyint/":{"title":"Tinyint","summary":"Tinyint <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Tinyint/","version":"local"},"/api/Cassandra/class.Tuple/":{"title":"Tuple","summary":"Tuple <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Tuple/","version":"local"},"/api/Cassandra/class.Type/":{"title":"Type","summary":"Type <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Type/","version":"local"},"/api/Cassandra/Type/class.Collection/":{"title":"Collection","summary":"Collection <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Type/class.Collection/","version":"local"},"/api/Cassandra/Type/class.Custom/":{"title":"Custom","summary":"Custom <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Type/class.Custom/","version":"local"},"/api/Cassandra/Type/class.Map/":{"title":"Map","summary":"Map <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Type/class.Map/","version":"local"},"/api/Cassandra/Type/class.Scalar/":{"title":"Scalar","summary":"Scalar <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Type/class.Scalar/","version":"local"},"/api/Cassandra/Type/class.Set/":{"title":"Set","summary":"Set <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Type/class.Set/","version":"local"},"/api/Cassandra/Type/class.Tuple/":{"title":"Tuple","summary":"Tuple <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Type/class.Tuple/","version":"local"},"/api/Cassandra/Type/class.UserType/":{"title":"UserType","summary":"UserType <small class=\"text-muted\">class</small>","path":"/api/Cassandra/Type/class.UserType/","version":"local"},"/api/Cassandra/class.UserTypeValue/":{"title":"UserTypeValue","summary":"UserTypeValue <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.UserTypeValue/","version":"local"},"/api/Cassandra/class.Uuid/":{"title":"Uuid","summary":"Uuid <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Uuid/","version":"local"},"/api/Cassandra/class.Varint/":{"title":"Varint","summary":"Varint <small class=\"text-muted\">class</small>","path":"/api/Cassandra/class.Varint/","version":"local"},"/api/class.DefaultFunction/":{"title":"DefaultFunction","summary":"DefaultFunction <small class=\"text-muted\">class</small>","path":"/api/class.DefaultFunction/","version":"local"},"/api/Cassandra/interface.Aggregate/":{"title":"Aggregate","summary":"Aggregate <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Aggregate/","version":"local"},"/api/Cassandra/interface.Cluster/":{"title":"Cluster","summary":"Cluster <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Cluster/","version":"local"},"/api/Cassandra/interface.Column/":{"title":"Column","summary":"Column <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Column/","version":"local"},"/api/Cassandra/interface.Exception/":{"title":"Exception","summary":"Exception <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Exception/","version":"local"},"/api/Cassandra/interface.Function/":{"title":"Function","summary":"Function <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Function/","version":"local"},"/api/Cassandra/interface.Future/":{"title":"Future","summary":"Future <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Future/","version":"local"},"/api/Cassandra/interface.Index/":{"title":"Index","summary":"Index <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Index/","version":"local"},"/api/Cassandra/interface.Keyspace/":{"title":"Keyspace","summary":"Keyspace <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Keyspace/","version":"local"},"/api/Cassandra/interface.MaterializedView/":{"title":"MaterializedView","summary":"MaterializedView <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.MaterializedView/","version":"local"},"/api/Cassandra/interface.Numeric/":{"title":"Numeric","summary":"Numeric <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Numeric/","version":"local"},"/api/Cassandra/interface.RetryPolicy/":{"title":"RetryPolicy","summary":"RetryPolicy <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.RetryPolicy/","version":"local"},"/api/Cassandra/interface.Schema/":{"title":"Schema","summary":"Schema <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Schema/","version":"local"},"/api/Cassandra/interface.Session/":{"title":"Session","summary":"Session <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Session/","version":"local"},"/api/Cassandra/interface.Statement/":{"title":"Statement","summary":"Statement <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Statement/","version":"local"},"/api/Cassandra/interface.Table/":{"title":"Table","summary":"Table <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Table/","version":"local"},"/api/Cassandra/interface.TimestampGenerator/":{"title":"TimestampGenerator","summary":"TimestampGenerator <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.TimestampGenerator/","version":"local"},"/api/Cassandra/interface.UuidInterface/":{"title":"UuidInterface","summary":"UuidInterface <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.UuidInterface/","version":"local"},"/api/Cassandra/interface.Value/":{"title":"Value","summary":"Value <small class=\"text-muted\">interface</small>","path":"/api/Cassandra/interface.Value/","version":"local"},"/api/Cassandra/":{"title":"Cassandra","summary":"Cassandra <small class=\"text-muted\">namespace</small>","path":"/api/Cassandra/","version":"local"},"/api/Cassandra/Cluster/":{"title":"Cluster","summary":"Cluster <small class=\"text-muted\">namespace</small>","path":"/api/Cassandra/Cluster/","version":"local"},"/api/Cassandra/Exception/":{"title":"Exception","summary":"Exception <small class=\"text-muted\">namespace</small>","path":"/api/Cassandra/Exception/","version":"local"},"/api/Cassandra/RetryPolicy/":{"title":"RetryPolicy","summary":"RetryPolicy <small class=\"text-muted\">namespace</small>","path":"/api/Cassandra/RetryPolicy/","version":"local"},"/api/Cassandra/SSLOptions/":{"title":"SSLOptions","summary":"SSLOptions <small class=\"text-muted\">namespace</small>","path":"/api/Cassandra/SSLOptions/","version":"local"},"/api/Cassandra/TimestampGenerator/":{"title":"TimestampGenerator","summary":"TimestampGenerator <small class=\"text-muted\">namespace</small>","path":"/api/Cassandra/TimestampGenerator/","version":"local"},"/api/Cassandra/Type/":{"title":"Type","summary":"Type <small class=\"text-muted\">namespace</small>","path":"/api/Cassandra/Type/","version":"local"},"/api/":{"title":"API docs","summary":"API docs <small class=\"text-muted\">index</small>","path":"/api/","version":"local"}})
  app.factory('basePath', basePath)
  app.provider('search', function() {
    function localSearchFactory($http, $timeout, $q, $rootScope, basePath) {
      $rootScope.searchReady = false;

      var fetch = $http.get(basePath + '/json/search-index.json')
                       .then(function(response) {
                         var index = lunr.Index.load(response.data)
                         $rootScope.searchReady = true;
                         return index;
                       });

      // The actual service is a function that takes a query string and
      // returns a promise to the search results
      // (In this case we just resolve the promise immediately as it is not
      // inherently an async process)
      return function(q) {
        return fetch.then(function(index) {
          var results = []
          index.search(q).forEach(function(hit) {
            results.push(hit.ref);
          });
          return results;
        })
      };
    };
    localSearchFactory.$inject = ['$http', '$timeout', '$q', '$rootScope', 'basePath'];

    function webWorkerSearchFactory($q, $rootScope, basePath) {
      $rootScope.searchReady = false;

      var searchIndex = $q.defer();
      var results;
      var worker = new Worker(basePath + '/js/search-worker.js');

      // The worker will send us a message in two situations:
      // - when the index has been built, ready to run a query
      // - when it has completed a search query and the results are available
      worker.onmessage = function(e) {
        switch(e.data.e) {
          case 'ready':
            worker.postMessage({ e: 'load', p: basePath });
            break
          case 'index-ready':
            $rootScope.$apply(function() {
              $rootScope.searchReady = true;
            })
            searchIndex.resolve();
            break;
          case 'query-ready':
            results.resolve(e.data.d);
            break;
        }
      };

      // The actual service is a function that takes a query string and
      // returns a promise to the search results
      return function(q) {

        // We only run the query once the index is ready
        return searchIndex.promise.then(function() {

          results = $q.defer();
          worker.postMessage({ e: 'search', q: q });
          return results.promise;
        });
      };
    };
    webWorkerSearchFactory.$inject = ['$q', '$rootScope', 'basePath'];

    return {
      $get: window.Worker ? webWorkerSearchFactory : localSearchFactory
    };
  })

  app.controller('search', [
    '$scope',
    '$sce',
    '$timeout',
    'search',
    'pages',
    'basePath',
    function($scope, $sce, $timeout, search, pages, basePath) {
      $scope.hasResults = false;
      $scope.results = null;
      $scope.current = null;

      function clear() {
        $scope.hasResults = false;
        $scope.results = null;
        $scope.current = null;
      }

      $scope.search = function(version) {
        if ($scope.q.length >= 2) {
          search($scope.q).then(function(ids) {
            var results = []

            ids.forEach(function(id) {
              var page = pages[id];

              if (page.version == version) {
                results.push(page)
              }
            })

            if (results.length > 0) {
              $scope.hasResults = true;
              $scope.results = results;
              $scope.current = 0;
            } else {
              clear()
            }
          })
        } else {
          clear()
        }
      };

      $scope.basePath = basePath;

      $scope.reset = function() {
        $scope.q = null;
        clear()
      }

      $scope.submit = function() {
        var result = $scope.results[$scope.current]

        if (result) {
          $timeout(function() {
            window.location.href = basePath + result.path;
          })
        }
      }

      $scope.summary = function(item) {
        return $sce.trustAsHtml(item.summary);
      }

      $scope.moveDown = function(e) {
        if ($scope.hasResults && $scope.current < ($scope.results.length - 1)) {
          $scope.current++
          e.stopPropagation()
        }
      }

      $scope.moveUp = function(e) {
        if ($scope.hasResults && $scope.current > 0) {
          $scope.current--
          e.stopPropagation()
        }
      }
    }
  ])

  app.directive('search', [
    '$document',
    'hotkeys',
    function($document, hotkeys) {
      return function(scope, element, attrs) {
        hotkeys.add({
          combo: '/',
          description: 'Search docs...',
          callback: function(event, hotkey) {
            event.preventDefault()
            event.stopPropagation()
            element[0].focus()
          }
        })
      }
    }
  ])

  $(function() {
    $('#content').height(
      Math.max(
        $(".side-nav").height(),
        $('#content').height()
      )
    );

    $('#table-of-contents').on('activate.bs.scrollspy', function() {
      var active = $('#table-of-contents li.active').last().children('a');
      var button = $('#current-section');
      var text   = active.text().trim();

      if (active.length == 0 || text == 'Page Top') {
        button.html('Jump to... <span class="caret"></span><span class="sr-only">Table of Contents</span>')
      } else {
        if (text.length > 30) {
          text = text.slice(0, 30) + '...'
        }
        button.html('Viewing: ' + text + ' <span class="caret"></span><span class="sr-only">Table of Contents</span>')
      }
    })

    // Config ZeroClipboard
    ZeroClipboard.config({
      swfPath: basePath() + '/flash/ZeroClipboard.swf',
      hoverClass: 'btn-clipboard-hover',
      activeClass: 'btn-clipboard-active'
    })

    // Insert copy to clipboard button before .highlight
    $('.highlight').each(function () {
      var btnHtml = '<div class="zero-clipboard"><span class="btn-clipboard">Copy</span></div>'
      $(this).before(btnHtml)
    })

    var zeroClipboard = new ZeroClipboard($('.btn-clipboard'))

    // Handlers for ZeroClipboard

    // Copy to clipboard
    zeroClipboard.on('copy', function (event) {
      var clipboard = event.clipboardData;
      var highlight = $(event.target).parent().nextAll('.highlight').first()
      clipboard.setData('text/plain', highlight.text())
    })

    // Notify copy success and reset tooltip title
    zeroClipboard.on('aftercopy', function (event) {
      $(event.target)
        .attr('title', 'Copied!')
        .tooltip('fixTitle')
        .tooltip('show')
    })

    // Notify copy failure
    zeroClipboard.on('error', function (event) {
      $(event.target)
        .attr('title', 'Flash required')
        .tooltip('fixTitle')
        .tooltip('show')
    })
  })
})(window)
