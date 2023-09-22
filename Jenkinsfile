#!groovy

def initializeEnvironment() {
  env.DRIVER_DISPLAY_NAME = 'Cassandra PHP Driver'
  env.DRIVER_METRIC_TYPE = 'oss'
  env.DRIVER_LIBRARY = 'cassandra'
  if (env.GIT_URL.contains('php-dse-driver')) {
    env.DRIVER_DISPLAY_NAME = 'DSE PHP Driver'
    env.DRIVER_TYPE = 'DSE'
    env.DRIVER_METRIC_TYPE = 'dse'
    env.DRIVER_LIBRARY = 'dse'
  }

  env.DRIVER_VERSION = sh(label: 'Determine driver version', script: '''#!/bin/bash -le
    driver_version=$(grep "#define[ \\t]\\+PHP_DRIVER_\\(MAJOR\\|MINOR\\|RELEASE\\|STABILITY\\)" ext/version.h | awk '
      BEGIN { major="?"; minor="?"; patch="?" }
      /_MAJOR/ { major=$3 }
      /_MINOR/ { minor=$3 }
      /_RELEASE/ { release=$3 }
      /_STABILITY/ { stability=$3; gsub(/"/, "", stability) }
      END {
        if (length(stability) > 0)
          printf "%s.%s.%s-%s", major, minor, release, stability
        else
          printf "%s.%s.%s", major, minor, release
      }
    ')
    if [[ ! ${driver_version} =~ ^[0-9]+\\.[0-9]+\\.[0-9]+([a-zA-Z0-9_\\-]+)?$ ]]
    then
      echo "Unable to extract version from ext/version.h"
      exit 1
    fi
    echo "${driver_version}"''', returnStdout: true).trim()

  env.GIT_SHA = "${env.GIT_COMMIT.take(7)}"
  env.GITHUB_PROJECT_URL = "https://${GIT_URL.replaceFirst(/(git@|http:\/\/|https:\/\/)/, '').replace(':', '/').replace('.git', '')}"
  env.GITHUB_BRANCH_URL = "${GITHUB_PROJECT_URL}/tree/${env.BRANCH_NAME}"
  env.GITHUB_COMMIT_URL = "${GITHUB_PROJECT_URL}/commit/${env.GIT_COMMIT}"

  if (env.IS_EXECUTING_INTEGRATION_TESTS == 'true') {
    sh label: 'Download Apache Cassandra&reg; or DataStax Enterprise', script: '''#!/bin/bash -le
      . ${CCM_ENVIRONMENT_SHELL} ${SERVER_VERSION}
    '''
  }

  sh label: 'Display C++ and PHP version along with environment information', script: '''#!/bin/bash -le
    # Load CCM environment variables
    if [ -f ${HOME}/environment.txt ]; then
      set -o allexport
      . ${HOME}/environment.txt
      set +o allexport
    fi

    if [ "${OS_DISTRO}" = "osx" ]; then
      clang --version
    else
      g++ --version
    fi
    cmake --version
    php${PHP_VERSION} --version
    printenv | sort
  '''
}

def installDependencies() {
  sh label: 'Build the DataStax C/C++ driver packages', script: '''#!/bin/bash -le
    (
      cd lib/cpp-driver/packaging
      ./build_deb.sh
      mkdir -p packages
      find build -type f -name "*.deb" -exec mv {} packages \\;
    )
  '''
  sh label: 'Install the DataStax C/C++ driver', script: '''#!/bin/bash -le
    sudo dpkg -i lib/cpp-driver/packaging/packages/*
  '''
}

def buildDriver() {
  sh label: 'Prepare and configre the build environment for the PHP extension', script: '''#!/bin/bash -le
    (
      cd ext
      phpize${PHP_VERSION}
    )

    mkdir -p build
    (
      cd build
      ../ext/configure --with-libdir=lib/x86_64-linux-gnu --with-php-config=php-config${PHP_VERSION}
    )
  '''

  sh label: 'Build the DataStax PHP driver extension', script: '''#!/bin/bash -le
    (
      cd build
      make -j$(nproc)
    )

    mkdir -p ${PHP_VERSION}
    cp build/modules/${DRIVER_LIBRARY}.so ${PHP_VERSION}
  '''

  sh label: 'Install the DataStax PHP driver extension', script: '''#!/bin/bash -le
    (
      cd build
      sudo make -j$(nproc) install
    )
  '''
}

def configureTestingEnvironment() {
  sh label: 'Configure PHP to load the DataStax PHP driver and configure logging', script: '''#!/bin/bash -le
    PHP_INI_FILE=$(php${PHP_VERSION} --ini | grep "Loaded Configuration" | sed -e "s|.*:\\s*||");
    sudo bash -c "cat >> ${PHP_INI_FILE}" << INI_EOF

# PHP driver extension configuration
extension=${DRIVER_LIBRARY}.so
date.timezone=America/Los_Angeles
cassandra.log=${WORKSPACE}/${PHP_VERSION}/${DRIVER_LIBRARY}-${SERVER_VERSION}.log
cassandra.log_level=TRACE
INI_EOF
    cat ${PHP_INI_FILE}
  '''

  sh label: 'Ensure DataStax PHP driver extension/module is loaded', script: '''#!/bin/bash -le
    php${PHP_VERSION} -i | grep -A 11 "^${DRIVER_LIBRARY}$"
  '''

  sh label: 'Install depencies for composer', script: '''#!/bin/bash -le
    composer install -n
  '''
}

def buildDocuments() {
  sh label: 'Building documents with documentor', script: '''#!/bin/bash -le
    documentor --output-directory ${HOME}/docs/drivers/php .
  '''

  sh label: 'Archive documentor generated documents', script: '''#!/bin/bash -le
    (
      cd ${HOME}/docs/drivers/php
      prefix=php-driver
      if [ "${DRIVER_TYPE}" = 'DSE' ]; then
        prefix=php-dse-driver
      fi
      tar czf ${WORKSPACE}/${prefix}-documents.tgz -C ${HOME}/docs/drivers/php .
    )
  '''
}

def executeUnitTests() {
  sh label: 'Execute quality assurance tests', script: '''#!/bin/bash -le
    (
      cd build
      make test
    )
  '''

  sh label: 'Execute unit tests', script: '''#!/bin/bash -le
    php${PHP_VERSION} bin/phpunit --log-junit ${DRIVER_TYPE}-unit-tests-${PHP_VERSION}-results.xml --testsuite unit
  '''
}

def executeIntegrationTests() {
  timeout(time: 5, unit: 'HOURS') {
    sh label: 'Execute integration tests', script: '''#!/bin/bash -le
      # Load CCM environment variables
      set -o allexport
      . ${HOME}/environment.txt
      set +o allexport

      if [ "${CCM_IS_DSE}" = 'true' ];then
        export DSE='true'
      fi
      export VERSION=${CCM_VERSION}

      php${PHP_VERSION} bin/phpunit --log-junit ${DRIVER_TYPE}-integration-tests-${PHP_VERSION}-results.xml --testsuite integration
    '''
  }
}

def determineBuildType() {
  if (params.ADHOC_BUILD_TYPE == 'BUILD-AND-EXECUTE-TESTS') {
    return 'adhoc'
  } else if (params.ADHOC_BUILD_TYPE == 'BUILD-DOCUMENTS') {
    return 'documents'
  }

  def buildType = 'commit'
  if (params.CI_SCHEDULE != 'DO-NOT-CHANGE-THIS-SELECTION') {
    buildType = "${params.CI_SCHEDULE.toLowerCase()}"
  }
  return buildType
}

def notifySlack(status = 'started') {
  // Notify Slack channel for every build except adhoc executions
  if (params.ADHOC_BUILD_TYPE != 'BUILD-AND-EXECUTE-TESTS') {
    // Set the global pipeline scoped environment (this is above each matrix)
    env.BUILD_STATED_SLACK_NOTIFIED = 'true'

    def buildType = determineBuildType()
    buildType = buildType.capitalize()

    def color = 'good' // Green
    if (status.equalsIgnoreCase('aborted')) {
      color = '808080' // Grey
    } else if (status.equalsIgnoreCase('unstable')) {
      color = 'warning' // Orange
    } else if (status.equalsIgnoreCase('failed')) {
      color = 'danger' // Red
    }

    def message = """Build ${status} for ${env.DRIVER_DISPLAY_NAME} [${buildType}]
<${env.GITHUB_BRANCH_URL}|${env.BRANCH_NAME}> - <${env.RUN_DISPLAY_URL}|#${env.BUILD_NUMBER}> - <${env.GITHUB_COMMIT_URL}|${env.GIT_SHA}>"""
    if (params.CI_SCHEDULE != 'DO-NOT-CHANGE-THIS-SELECTION') {
      message += " - ${env.OS_VERSION}"
    }
    if (!status.equalsIgnoreCase('Started')) {
      message += """
${status} after ${currentBuild.durationString - ' and counting'}"""
    }

    slackSend color: "${color}",
              channel: "#cpp-driver-dev-bots",
              message: "${message}"
  }
}

def submitCIMetrics(buildType) {
  long durationMs = currentBuild.duration
  long durationSec = durationMs / 1000
  long nowSec = (currentBuild.startTimeInMillis + durationMs) / 1000
  def branchNameNoPeriods = env.BRANCH_NAME.replaceAll('\\.', '_')
  def durationMetric = "okr.ci.php.${env.DRIVER_METRIC_TYPE}.${buildType}.${branchNameNoPeriods} ${durationSec} ${nowSec}"

  timeout(time: 1, unit: 'MINUTES') {
    withCredentials([string(credentialsId: 'lab-grafana-address', variable: 'LAB_GRAFANA_ADDRESS'),
                     string(credentialsId: 'lab-grafana-port', variable: 'LAB_GRAFANA_PORT')]) {
      withEnv(["DURATION_METRIC=${durationMetric}"]) {
        sh label: 'Send runtime metrics to labgrafana', script: '''#!/bin/bash -le
          echo "${DURATION_METRIC}" | nc -q 5 ${LAB_GRAFANA_ADDRESS} ${LAB_GRAFANA_PORT}
        '''
      }
    }
  }
}

def describePerCommitStage() {
  script {
    currentBuild.displayName = "Per-Commit build of ${env.BRANCH_NAME}"
    currentBuild.description = "Per-Commit build, quality assurance, and unit testing for branch ${env.BRANCH_NAME}"
  }
}

def describeScheduledAndAdhocTestingStage() {
  script {
    if (params.CI_SCHEDULE != 'DO-NOT-CHANGE-THIS-SELECTION') {
      currentBuild.displayName = "${params.CI_SCHEDULE.toLowerCase().capitalize()} schedule"
    } else {
      currentBuild.displayName = "${params.ADHOC_BUILD_AND_EXECUTE_TESTS_SERVER_VERSION} adhoc"
    }

    def serverVersions = env.SERVER_VERSIONS.split(' ')
    def serverDisplayNames = ''
    serverVersions.each {
      def serverType = it.split('-')[0]
      def serverDisplayName = 'Apache Cassandra&reg;'
      if (serverType == 'ALL') {
        serverDisplayName = "all Apache Cassandra&reg; and DataStax Enterprise server versions"
      } else {
        def serverVersion = it
        try {
          serverVersion = it.split('-')[1]
        } catch (e) {
          ;; // no-op
        }
        if (serverType == 'ddac') {
          serverDisplayName = "DataStax Distribution of ${serverDisplayName}"
        } else if (serverType == 'dse') {
          serverDisplayName = 'DataStax Enterprise'
        }
        serverDisplayNames += "${serverDisplayName} v${serverVersion}.x"
        if (it != serverVersions[-1]) {
          serverDisplayNames += ', '
          if (it != serverVersions[-2]) {
            serverDisplayNames += 'and '
          }
        }
      }
    }
    currentBuild.description = "Testing ${serverDisplayNames}"
  }
}

def describeScheduledAndAdhocBuildDocuments() {
  script {
    currentBuild.displayName = "Build documents [${env.GIT_SHA}]"
    currentBuild.description = "Build the DataStax PHP driver documents [${env.GIT_SHA}]"
  }
}

pipeline {
  agent none

  // Global pipeline timeout
  options {
    timeout(time: 10, unit: 'HOURS')
    buildDiscarder(logRotator(artifactNumToKeepStr: '10', // Keep only the last 10 artifacts
                              numToKeepStr: '50'))        // Keep only the last 50 build records
  }

  parameters {
    choice(
      name: 'ADHOC_BUILD_TYPE',
      choices: ['BUILD', 'BUILD-AND-EXECUTE-TESTS', 'BUILD-DOCUMENTS'],
      description: '''Perform a adhoc build operation
                      <table style="width:100%">
                        <col width="25%">
                        <col width="75%">
                        <tr>
                          <th align="left">Choice</th>
                          <th align="left">Description</th>
                        </tr>
                        <tr>
                          <td><strong>BUILD</strong></td>
                          <td>Performs a <b>Per-Commit</b> build</td>
                        </tr>
                        <tr>
                          <td><strong>BUILD-AND-EXECUTE-TESTS</strong></td>
                          <td>
                            Performs a build and executes the integration and unit tests<br/>
                            <ul>
                              <li>Use <PHP_VERSION</b> to change PHP selection</li>
                              <li>Use <ADHOC_BUILD_AND_EXECUTE_TESTS_SERVER_VERSION</b> to change Apache Cassandra&reg; and DataStax Enterprise selection</li>
                            </ul>
                          </td>
                        </tr>
                        <tr>
                          <td><strong>BUILD-DOCUMENTS</strong></td>
                          <td>Performs a document build using documentor</td>
                        </tr>
                      </table>
                      <br/>''')
    choice(
      name: 'PHP_VERSION',
      choices: ['7.2',
                '7.3',
                '7.4'],
      description: '''PHP version to use for adhoc <b>BUILD-AND-EXECUTE-TESTS</b> builds
                      <table style="width:100%">
                        <col width="15%">
                        <col width="85%">
                        <tr>
                          <th align="left">Choice</th>
                          <th align="left">Description</th>
                        </tr>
                        <tr>
                          <td><strong>7.2</strong></td>
                          <td>PHP v7.2.x</td>
                        </tr>
                        <tr>
                          <td><strong>7.3</strong></td>
                          <td>PHP v7.3.x</td>
                        </tr>
                        <tr>
                          <td><strong>7.4</strong></td>
                          <td>PHP v7.4.x</td>
                        </tr>
                      </table>''')
    choice(
      name: 'ADHOC_BUILD_AND_EXECUTE_TESTS_SERVER_VERSION',
      choices: ['2.1',      // Legacy Apache Cassandra®
                '2.2',      // Legacy Apache Cassandra®
                '3.0',      // Previous Apache Cassandra®
                '3.11',     // Current Apache Cassandra®
                '4.0',      // Development Apache Cassandra®
                'ddac-5.1', // Current DataStax Distribution of Apache Cassandra®
                'dse-4.8',  // Previous EOSL DataStax Enterprise
                'dse-5.0',  // Long Term Support DataStax Enterprise
                'dse-5.1',  // Legacy DataStax Enterprise
                'dse-6.0',  // Previous DataStax Enterprise
                'dse-6.7',  // Current DataStax Enterprise
                'dse-6.8',  // Development DataStax Enterprise
                'ALL'],
      description: '''Apache Cassandra&reg; and DataStax Enterprise server version to use for adhoc <b>BUILD-AND-EXECUTE-TESTS</b> builds
                      <table style="width:100%">
                        <col width="15%">
                        <col width="85%">
                        <tr>
                          <th align="left">Choice</th>
                          <th align="left">Description</th>
                        </tr>
                        <tr>
                          <td><strong>2.1</strong></td>
                          <td>Apache Cassandra&reg; v2.1.x</td>
                        </tr>
                        <tr>
                          <td><strong>2.2</strong></td>
                          <td>Apache Cassandra&reg; v2.2.x</td>
                        </tr>
                        <tr>
                          <td><strong>3.0</strong></td>
                          <td>Apache Cassandra&reg; v3.0.x</td>
                        </tr>
                        <tr>
                          <td><strong>3.11</strong></td>
                          <td>Apache Cassandra&reg; v3.11.x</td>
                        </tr>
                        <tr>
                          <td><strong>4.0</strong></td>
                          <td>Apache Cassandra&reg; v4.x (<b>CURRENTLY UNDER DEVELOPMENT</b>)</td>
                        </tr>
                        <tr>
                          <td><strong>ddac-5.1</strong></td>
                          <td>DataStax Distribution of Apache Cassandra&reg; v5.1.x</td>
                        </tr>
                        <tr>
                          <td><strong>dse-4.8</strong></td>
                          <td>DataStax Enterprise v4.8.x (<b>END OF SERVICE LIFE</b>)</td>
                        </tr>
                        <tr>
                          <td><strong>dse-5.0</strong></td>
                          <td>DataStax Enterprise v5.0.x (<b>Long Term Support</b>)</td>
                        </tr>
                        <tr>
                          <td><strong>dse-5.1</strong></td>
                          <td>DataStax Enterprise v5.1.x</td>
                        </tr>
                        <tr>
                          <td><strong>dse-6.0</strong></td>
                          <td>DataStax Enterprise v6.0.x</td>
                        </tr>
                        <tr>
                          <td><strong>dse-6.7</strong></td>
                          <td>DataStax Enterprise v6.7.x</td>
                        </tr>
                        <tr>
                          <td><strong>dse-6.8</strong></td>
                          <td>DataStax Enterprise v6.8.x (<b>CURRENTLY UNDER DEVELOPMENT</b>)</td>
                        </tr>
                      </table>''')
    choice(
      name: 'CI_SCHEDULE',
      choices: ['DO-NOT-CHANGE-THIS-SELECTION', 'WEEKNIGHTS'],
      description: 'CI testing schedule to execute periodically scheduled builds and tests of the driver (<strong>DO NOT CHANGE THIS SELECTION</strong>)')
    string(
      name: 'CI_SCHEDULE_SERVER_VERSIONS',
      defaultValue: 'DO-NOT-CHANGE-THIS-SELECTION',
      description: 'CI testing server version(s) to utilize for scheduled test runs of the driver (<strong>DO NOT CHANGE THIS SELECTION</strong>)')
  }

  triggers {
    parameterizedCron("""
      # Every weeknight (Monday - Friday) around 6:00 AM
      H 6 * * 1-5 %CI_SCHEDULE=WEEKNIGHTS;PHP_VERSIONS=7.2;CI_SCHEDULE_SERVER_VERSIONS=3.11 4.0
      H 6 * * 1-5 %CI_SCHEDULE=WEEKNIGHTS;PHP_VERSIONS=7.3;CI_SCHEDULE_SERVER_VERSIONS=3.11 4.0
      H 6 * * 1-5 %CI_SCHEDULE=WEEKNIGHTS;PHP_VERSIONS=7.4;CI_SCHEDULE_SERVER_VERSIONS=3.11 4.0
      # Every weekend (Sunday) around 8:00 AM
      H 8 * * 0 %CI_SCHEDULE=WEEKENDS;PHP_VERSIONS=7.2;CI_SCHEDULE_SERVER_VERSIONS=2.1 2.2 3.0
      H 8 * * 0 %CI_SCHEDULE=WEEKENDS;PHP_VERSIONS=7.3;CI_SCHEDULE_SERVER_VERSIONS=2.1 2.2 3.0
      H 8 * * 0 %CI_SCHEDULE=WEEKENDS;PHP_VERSIONS=7.4;CI_SCHEDULE_SERVER_VERSIONS=2.1 2.2 3.0
      H 8 * * 0 %CI_SCHEDULE=WEEKENDS;ADHOC_BUILD_TYPE=BUILD-DOCUMENTS
    """)
  }

  environment {
    CCM_ENVIRONMENT_SHELL = '/usr/local/bin/ccm_environment.sh'
    OS_VERSION = 'ubuntu/bionic64/php'
  }

  stages {
    stage('Per-Commit') {
      options {
        timeout(time: 1, unit: 'HOURS')
      }
      when {
        beforeAgent true
        allOf {
          expression { params.ADHOC_BUILD_TYPE == 'BUILD' }
          expression { params.CI_SCHEDULE == 'DO-NOT-CHANGE-THIS-SELECTION' }
          expression { params.CI_SCHEDULE_SERVER_VERSIONS == 'DO-NOT-CHANGE-THIS-SELECTION' }
        }
      }

      matrix {
        axes {
          axis {
            name 'PHP_VERSION'
            values '7.2',
                   '7.3',
                   '7.4'
          }
        }

        agent {
          label "${env.OS_VERSION}"
        }

        stages {
          stage('Initialize-Environment') {
            steps {
              initializeEnvironment()
              script {
                if (env.BUILD_STATED_SLACK_NOTIFIED != 'true') {
                  notifySlack()
                }
              }
            }
          }
          stage('Describe-Build') {
            steps {
              describePerCommitStage()
            }
          }
          stage('Install-Dependencies') {
            steps {
              installDependencies()
            }
          }
          stage('Build-Driver') {
            steps {
              buildDriver()
            }
            post {
              success {
                archiveArtifacts artifacts: "${env.PHP_VERSION}/*.so"
              }
            }
          }
          stage('Execute-Unit-Tests') {
            steps {
              configureTestingEnvironment()
              executeUnitTests()
            }
            post {
              always {
                // Allow empty results if segfault occurs
                junit testResults: '*unit-tests-*-results.xml', allowEmptyResults: true
              }
            }
          }
        }
      }
      post {
        always {
          node('master') {
            submitCIMetrics('commit')
          }
        }
        aborted {
          notifySlack('aborted')
        }
        success {
          notifySlack('completed')
        }
        unstable {
          notifySlack('unstable')
        }
        failure {
          notifySlack('FAILED')
        }
      }
    }

    stage('Scheduled-And-Adhoc-Testing') {
      when {
        beforeAgent true
        allOf {
          not { buildingTag() }
          anyOf {
            expression { params.ADHOC_BUILD_TYPE == 'BUILD-AND-EXECUTE-TESTS' }
            allOf {
              expression { params.ADHOC_BUILD_TYPE == 'BUILD' }
              expression { params.CI_SCHEDULE != 'DO-NOT-CHANGE-THIS-SELECTION' }
              expression { params.CI_SCHEDULE_SERVER_VERSIONS != 'DO-NOT-CHANGE-THIS-SELECTION' }
            }
          }
        }
      }

      environment {
        IS_EXECUTING_INTEGRATION_TESTS = 'true'
        PHP_VERSION = "${params.PHP_VERSION}"
        SERVER_VERSIONS = "${params.CI_SCHEDULE_SERVER_VERSIONS == 'DO-NOT-CHANGE-THIS-SELECTION' ? params.ADHOC_BUILD_AND_EXECUTE_TESTS_SERVER_VERSION : params.CI_SCHEDULE_SERVER_VERSIONS}"
      }

      matrix {
        axes {
          axis {
            name 'SERVER_VERSION'
            values '2.1',      // Legacy Apache Cassandra®
                   '2.2',      // Legacy Apache Cassandra®
                   '3.0',      // Previous Apache Cassandra®
                   '3.11',     // Current Apache Cassandra®
                   '4.0',      // Development Apache Cassandra®
                   'ddac-5.1', // Current DataStax Distribution of Apache Cassandra®
                   'dse-4.8',  // Previous EOSL DataStax Enterprise
                   'dse-5.0',  // Long Term Support DataStax Enterprise
                   'dse-5.1',  // Legacy DataStax Enterprise
                   'dse-6.0',  // Previous DataStax Enterprise
                   'dse-6.7',  // Current DataStax Enterprise
                   'dse-6.8'   // Development DataStax Enterprise
          }
        }
        when {
          beforeAgent true
          allOf {
            expression { return env.SERVER_VERSIONS.split(' ').any { it =~ /(ALL|${env.SERVER_VERSION})/ } }
          }
        }

        agent {
          label "${env.OS_VERSION}"
        }

        stages {
          stage('Initialize-Environment') {
            steps {
              initializeEnvironment()
              script {
                if (env.BUILD_STATED_SLACK_NOTIFIED != 'true') {
                  notifySlack()
                }
              }
            }
          }
          stage('Describe-Build') {
            steps {
              describeScheduledAndAdhocTestingStage()
            }
          }
          stage('Install-Dependencies') {
            steps {
              installDependencies()
            }
          }
          stage('Build-Driver') {
            steps {
              buildDriver()
            }
            post {
              success {
                archiveArtifacts artifacts: "${env.PHP_VERSION}/*.so"
              }
            }
          }
          stage('Execute-Integration-Tests') {
            steps {
              configureTestingEnvironment()
              executeIntegrationTests()
            }
            post {
              always {
                // Allow empty results for when segfaults occur
                junit testResults: '*integration-tests-*-results.xml', allowEmptyResults: true
              }
              failure {
                archiveArtifacts artifacts: "${env.PHP_VERSION}/*.log"
              }
            }
          }
        }
      }
      post {
        aborted {
          notifySlack('aborted')
        }
        success {
          notifySlack('completed')
        }
        unstable {
          notifySlack('unstable')
        }
        failure {
          notifySlack('FAILED')
        }
      }
    }

    stage('Scheduled-And-Adhoc-Build-Documents') {
      when {
        beforeAgent true
        allOf {
          not { buildingTag() }
          anyOf {
            allOf {
              // User initiated
              expression { params.ADHOC_BUILD_TYPE == 'BUILD-DOCUMENTS' }
              expression { params.CI_SCHEDULE == 'DO-NOT-CHANGE-THIS-SELECTION' }
              expression { params.CI_SCHEDULE_SERVER_VERSIONS == 'DO-NOT-CHANGE-THIS-SELECTION' }
            }
            allOf {
              // Schedule initiated
              branch 'master'
              expression { params.ADHOC_BUILD_TYPE == 'BUILD-DOCUMENTS' }
              expression { params.CI_SCHEDULE == 'WEEKENDS' }
              expression { params.CI_SCHEDULE_SERVER_VERSIONS == 'DO-NOT-CHANGE-THIS-SELECTION' }
            }
          }
        }
      }

      agent {
        label "${env.OS_VERSION}"
      }

      stages {
        stage('Initialize-Environment') {
          steps {
            initializeEnvironment()
            script {
              notifySlack()
            }
          }
        }
        stage('Describe-Build') {
          steps {
            describeScheduledAndAdhocBuildDocuments()
          }
        }
        stage('Build-Documents') {
          steps {
            buildDocuments()
          }
          post {
            success {
              archiveArtifacts artifacts: '*-documents.tgz'
            }
          }
        }
      }
      post {
        aborted {
          notifySlack('aborted')
        }
        success {
          notifySlack('completed')
        }
        unstable {
          notifySlack('unstable')
        }
        failure {
          notifySlack('FAILED')
        }
      }
    }
  }
}