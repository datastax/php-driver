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

  app.value('pages', {"/":{"title":"Home","summary":"Home <small class=\"text-muted\">section</small>","path":"/","version":"current"},"/features/":{"title":"Features","summary":"Features <small class=\"text-muted\">section</small>","path":"/features/","version":"current"},"/features/batch_statements/":{"title":"Batch statements","summary":"Batch statements <small class=\"text-muted\">page</small>","path":"/features/batch_statements/","version":"current"},"/features/datatypes/":{"title":"Datatypes","summary":"Datatypes <small class=\"text-muted\">page</small>","path":"/features/datatypes/","version":"current"},"/features/prepared_statements/":{"title":"Prepared Statements","summary":"Prepared Statements <small class=\"text-muted\">page</small>","path":"/features/prepared_statements/","version":"current"},"/features/result_paging/":{"title":"Result paging","summary":"Result paging <small class=\"text-muted\">page</small>","path":"/features/result_paging/","version":"current"},"/features/session_management/":{"title":"Session management","summary":"Session management <small class=\"text-muted\">page</small>","path":"/features/session_management/","version":"current"},"/features/simple_statements/":{"title":"Simple Statements","summary":"Simple Statements <small class=\"text-muted\">page</small>","path":"/features/simple_statements/","version":"current"},"/features/ssl_encryption/":{"title":"SSL encryption","summary":"SSL encryption <small class=\"text-muted\">page</small>","path":"/features/ssl_encryption/","version":"current"},"/api/class/Cassandra/":{"title":"Cassandra","summary":"Cassandra <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/","version":"current"},"/api/class/Cassandra/BatchStatement/":{"title":"BatchStatement","summary":"BatchStatement <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/BatchStatement/","version":"current"},"/api/class/Cassandra/Bigint/":{"title":"Bigint","summary":"Bigint <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Bigint/","version":"current"},"/api/class/Cassandra/Blob/":{"title":"Blob","summary":"Blob <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Blob/","version":"current"},"/api/class/Cassandra/Cluster/Builder/":{"title":"Builder","summary":"Builder <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Cluster/Builder/","version":"current"},"/api/class/Cassandra/Collection/":{"title":"Collection","summary":"Collection <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Collection/","version":"current"},"/api/class/Cassandra/Decimal/":{"title":"Decimal","summary":"Decimal <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Decimal/","version":"current"},"/api/class/Cassandra/DefaultCluster/":{"title":"DefaultCluster","summary":"DefaultCluster <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/DefaultCluster/","version":"current"},"/api/class/Cassandra/DefaultSession/":{"title":"DefaultSession","summary":"DefaultSession <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/DefaultSession/","version":"current"},"/api/class/Cassandra/Exception/AlreadyExistsException/":{"title":"AlreadyExistsException","summary":"AlreadyExistsException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/AlreadyExistsException/","version":"current"},"/api/class/Cassandra/Exception/AuthenticationException/":{"title":"AuthenticationException","summary":"AuthenticationException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/AuthenticationException/","version":"current"},"/api/class/Cassandra/Exception/ConfigurationException/":{"title":"ConfigurationException","summary":"ConfigurationException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/ConfigurationException/","version":"current"},"/api/class/Cassandra/Exception/DivideByZeroException/":{"title":"DivideByZeroException","summary":"DivideByZeroException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/DivideByZeroException/","version":"current"},"/api/class/Cassandra/Exception/DomainException/":{"title":"DomainException","summary":"DomainException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/DomainException/","version":"current"},"/api/class/Cassandra/Exception/ExecutionException/":{"title":"ExecutionException","summary":"ExecutionException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/ExecutionException/","version":"current"},"/api/class/Cassandra/Exception/InvalidArgumentException/":{"title":"InvalidArgumentException","summary":"InvalidArgumentException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/InvalidArgumentException/","version":"current"},"/api/class/Cassandra/Exception/InvalidQueryException/":{"title":"InvalidQueryException","summary":"InvalidQueryException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/InvalidQueryException/","version":"current"},"/api/class/Cassandra/Exception/InvalidSyntaxException/":{"title":"InvalidSyntaxException","summary":"InvalidSyntaxException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/InvalidSyntaxException/","version":"current"},"/api/class/Cassandra/Exception/IsBootstrappingException/":{"title":"IsBootstrappingException","summary":"IsBootstrappingException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/IsBootstrappingException/","version":"current"},"/api/class/Cassandra/Exception/LogicException/":{"title":"LogicException","summary":"LogicException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/LogicException/","version":"current"},"/api/class/Cassandra/Exception/OverloadedException/":{"title":"OverloadedException","summary":"OverloadedException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/OverloadedException/","version":"current"},"/api/class/Cassandra/Exception/ProtocolException/":{"title":"ProtocolException","summary":"ProtocolException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/ProtocolException/","version":"current"},"/api/class/Cassandra/Exception/RangeException/":{"title":"RangeException","summary":"RangeException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/RangeException/","version":"current"},"/api/class/Cassandra/Exception/ReadTimeoutException/":{"title":"ReadTimeoutException","summary":"ReadTimeoutException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/ReadTimeoutException/","version":"current"},"/api/class/Cassandra/Exception/RuntimeException/":{"title":"RuntimeException","summary":"RuntimeException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/RuntimeException/","version":"current"},"/api/class/Cassandra/Exception/ServerException/":{"title":"ServerException","summary":"ServerException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/ServerException/","version":"current"},"/api/class/Cassandra/Exception/TimeoutException/":{"title":"TimeoutException","summary":"TimeoutException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/TimeoutException/","version":"current"},"/api/class/Cassandra/Exception/TruncateException/":{"title":"TruncateException","summary":"TruncateException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/TruncateException/","version":"current"},"/api/class/Cassandra/Exception/UnauthorizedException/":{"title":"UnauthorizedException","summary":"UnauthorizedException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/UnauthorizedException/","version":"current"},"/api/class/Cassandra/Exception/UnavailableException/":{"title":"UnavailableException","summary":"UnavailableException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/UnavailableException/","version":"current"},"/api/class/Cassandra/Exception/UnpreparedException/":{"title":"UnpreparedException","summary":"UnpreparedException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/UnpreparedException/","version":"current"},"/api/class/Cassandra/Exception/ValidationException/":{"title":"ValidationException","summary":"ValidationException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/ValidationException/","version":"current"},"/api/class/Cassandra/Exception/WriteTimeoutException/":{"title":"WriteTimeoutException","summary":"WriteTimeoutException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Exception/WriteTimeoutException/","version":"current"},"/api/class/Cassandra/ExecutionOptions/":{"title":"ExecutionOptions","summary":"ExecutionOptions <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/ExecutionOptions/","version":"current"},"/api/class/Cassandra/Float/":{"title":"Float","summary":"Float <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Float/","version":"current"},"/api/class/Cassandra/FutureClose/":{"title":"FutureClose","summary":"FutureClose <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/FutureClose/","version":"current"},"/api/class/Cassandra/FutureException/":{"title":"FutureException","summary":"FutureException <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/FutureException/","version":"current"},"/api/class/Cassandra/FuturePreparedStatement/":{"title":"FuturePreparedStatement","summary":"FuturePreparedStatement <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/FuturePreparedStatement/","version":"current"},"/api/class/Cassandra/FutureRows/":{"title":"FutureRows","summary":"FutureRows <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/FutureRows/","version":"current"},"/api/class/Cassandra/FutureSession/":{"title":"FutureSession","summary":"FutureSession <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/FutureSession/","version":"current"},"/api/class/Cassandra/FutureValue/":{"title":"FutureValue","summary":"FutureValue <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/FutureValue/","version":"current"},"/api/class/Cassandra/Inet/":{"title":"Inet","summary":"Inet <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Inet/","version":"current"},"/api/class/Cassandra/Map/":{"title":"Map","summary":"Map <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Map/","version":"current"},"/api/class/Cassandra/PreparedStatement/":{"title":"PreparedStatement","summary":"PreparedStatement <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/PreparedStatement/","version":"current"},"/api/class/Cassandra/Rows/":{"title":"Rows","summary":"Rows <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Rows/","version":"current"},"/api/class/Cassandra/SSLOptions/":{"title":"SSLOptions","summary":"SSLOptions <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/SSLOptions/","version":"current"},"/api/class/Cassandra/SSLOptions/Builder/":{"title":"Builder","summary":"Builder <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/SSLOptions/Builder/","version":"current"},"/api/class/Cassandra/Set/":{"title":"Set","summary":"Set <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Set/","version":"current"},"/api/class/Cassandra/SimpleStatement/":{"title":"SimpleStatement","summary":"SimpleStatement <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/SimpleStatement/","version":"current"},"/api/class/Cassandra/Timestamp/":{"title":"Timestamp","summary":"Timestamp <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Timestamp/","version":"current"},"/api/class/Cassandra/Timeuuid/":{"title":"Timeuuid","summary":"Timeuuid <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Timeuuid/","version":"current"},"/api/class/Cassandra/Uuid/":{"title":"Uuid","summary":"Uuid <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Uuid/","version":"current"},"/api/class/Cassandra/Varint/":{"title":"Varint","summary":"Varint <small class=\"text-muted\">class</small>","path":"/api/class/Cassandra/Varint/","version":"current"},"/api/interface/Cassandra/Cluster/":{"title":"Cluster","summary":"Cluster <small class=\"text-muted\">interface</small>","path":"/api/interface/Cassandra/Cluster/","version":"current"},"/api/interface/Cassandra/Exception/":{"title":"Exception","summary":"Exception <small class=\"text-muted\">interface</small>","path":"/api/interface/Cassandra/Exception/","version":"current"},"/api/interface/Cassandra/Future/":{"title":"Future","summary":"Future <small class=\"text-muted\">interface</small>","path":"/api/interface/Cassandra/Future/","version":"current"},"/api/interface/Cassandra/Numeric/":{"title":"Numeric","summary":"Numeric <small class=\"text-muted\">interface</small>","path":"/api/interface/Cassandra/Numeric/","version":"current"},"/api/interface/Cassandra/Session/":{"title":"Session","summary":"Session <small class=\"text-muted\">interface</small>","path":"/api/interface/Cassandra/Session/","version":"current"},"/api/interface/Cassandra/Statement/":{"title":"Statement","summary":"Statement <small class=\"text-muted\">interface</small>","path":"/api/interface/Cassandra/Statement/","version":"current"},"/api/interface/Cassandra/UuidInterface/":{"title":"UuidInterface","summary":"UuidInterface <small class=\"text-muted\">interface</small>","path":"/api/interface/Cassandra/UuidInterface/","version":"current"},"/api/namespace/Cassandra/":{"title":"Cassandra","summary":"Cassandra <small class=\"text-muted\">namespace</small>","path":"/api/namespace/Cassandra/","version":"current"},"/api/namespace/Cassandra/Cluster/":{"title":"Cluster","summary":"Cluster <small class=\"text-muted\">namespace</small>","path":"/api/namespace/Cassandra/Cluster/","version":"current"},"/api/namespace/Cassandra/Exception/":{"title":"Exception","summary":"Exception <small class=\"text-muted\">namespace</small>","path":"/api/namespace/Cassandra/Exception/","version":"current"},"/api/namespace/Cassandra/SSLOptions/":{"title":"SSLOptions","summary":"SSLOptions <small class=\"text-muted\">namespace</small>","path":"/api/namespace/Cassandra/SSLOptions/","version":"current"},"/api/":{"title":"API docs","summary":"API docs <small class=\"text-muted\">index</small>","path":"/api/","version":"current"}})
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

    function togglePanelHeadingGlyph(e) {
      $(this).find('.glyphicon').toggleClass('glyphicon-chevron-up glyphicon-chevron-down');
    }

    $('#table-of-contents-panel').on('shown.bs.collapse', togglePanelHeadingGlyph);
    $('#table-of-contents-panel').on('hidden.bs.collapse', togglePanelHeadingGlyph);
    $('#read-more-panel').on('shown.bs.collapse', togglePanelHeadingGlyph);
    $('#read-more-panel').on('hidden.bs.collapse', togglePanelHeadingGlyph);

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
