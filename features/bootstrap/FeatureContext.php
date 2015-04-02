<?php

use Behat\Behat\Context\Context;
use Behat\Behat\Context\SnippetAcceptingContext;
use Behat\Gherkin\Node\PyStringNode;
use Behat\Gherkin\Node\TableNode;
use Symfony\Component\Process\PhpExecutableFinder;
use Symfony\Component\Process\Process;
use Behat\Behat\Tester\Exception\PendingException;

require_once __DIR__ . '/../../support/ccm.php';

/**
 * Defines application features from the specific context.
 */
class FeatureContext implements Context, SnippetAcceptingContext
{
    private $workingDir;
    private $phpBin;
    private $process;
    private $ccm;

    /**
     * Initializes context.
     *
     * Every scenario gets its own context instance.
     * You can also pass arbitrary arguments to the
     * context constructor through behat.yml.
     */
    public function __construct()
    {
        $this->ccm = new \CCM('php-driver-test-cluster', '2.1.3');
    }

    /**
     * Cleans test folders in the temporary directory.
     *
     * @BeforeSuite
     * @AfterSuite
     */
    public static function cleanTestFolders()
    {
        if (is_dir($dir = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'php-driver')) {
            self::clearDirectory($dir);
        }
    }

    /**
     * Prepares test folders in the temporary directory.
     *
     * @BeforeScenario
     */
    public function prepareTestFolders()
    {
        $dir = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'php-driver' . DIRECTORY_SEPARATOR .
        md5(microtime() * rand(0, 10000));
        mkdir($dir . '/features/bootstrap/i18n', 0777, true);
        $phpFinder = new PhpExecutableFinder();
        if (false === $php = $phpFinder->find()) {
            throw new \RuntimeException('Unable to find the PHP executable.');
        }
        $this->workingDir = $dir;
        $this->phpBin     = $php;
        $this->process    = new Process(null);
    }

    /**
     * @Given a running Cassandra cluster
     */
    public function aRunningCassandraCluster()
    {
        $this->ccm->setup();
        $this->ccm->start();
    }

    /**
     * @Given a running cassandra cluster with SSL encryption
     */
    public function aRunningCassandraClusterWithSslEncryption()
    {
        $this->ccm->setupSSL();
        $this->ccm->start();
    }

    /**
     * @Given a running cassandra cluster with client certificate verification
     */
    public function aRunningCassandraClusterWithClientCertificateVerification()
    {
      $this->ccm->setupClientVerification();
      $this->ccm->start();
    }

    /**
     * @Given /^the following schema:$/
     */
    public function theFollowingSchema(PyStringNode $string)
    {
        $this->ccm->setupSchema((string) $string);
    }

    /**
     * @Given /^the following example:$/
     */
    public function theFollowingExample(PyStringNode $string)
    {
        $this->createFile($this->workingDir . '/example.php', (string) $string);
    }

    /**
     * @When /^it is executed$/
     */
    public function itIsExecuted()
    {
        $this->process->setWorkingDirectory($this->workingDir);
        $this->process->setCommandLine(sprintf(
            '%s %s', $this->phpBin, 'example.php'
        ));
        // Don't reset the LANG variable on HHVM, because it breaks HHVM itself
        if (!defined('HHVM_VERSION')) {
            $env = $this->process->getEnv();
            $env['LANG'] = 'en'; // Ensures that the default language is en, whatever the OS locale is.
            $this->process->setEnv($env);
        }
        $this->process->start();
        $this->process->wait();
    }

    /**
     * @When it is executed with trusted cert in the env
     */
    public function itIsExecutedWithTrustedCertInTheEnv()
    {
      $this->process->setWorkingDirectory($this->workingDir);
      $this->process->setCommandLine(sprintf(
          '%s %s', $this->phpBin, 'example.php'
      ));
      $env = $this->process->getEnv();
      $env['SERVER_CERT'] = realpath(__DIR__ . '/../../support/ssl/cassandra.pem');
      // Don't reset the LANG variable on HHVM, because it breaks HHVM itself
      if (!defined('HHVM_VERSION')) {
          $env['LANG'] = 'en'; // Ensures that the default language is en, whatever the OS locale is.
      }
      $this->process->setEnv($env);
      $this->process->start();
      $this->process->wait();
    }

    /**
     * @When it is executed with trusted and client certs, private key and passphrase in the env
     */
    public function itIsExecutedWithTrustedAndClientCertsPrivateKeyAndPassphraseInTheEnv()
    {
        $this->process->setWorkingDirectory($this->workingDir);
        $this->process->setCommandLine(sprintf(
            '%s %s', $this->phpBin, 'example.php'
        ));
        $env = $this->process->getEnv();
        $env['SERVER_CERT'] = realpath(__DIR__ . '/../../support/ssl/cassandra.pem');
        $env['CLIENT_CERT'] = realpath(__DIR__ . '/../../support/ssl/driver.pem');
        $env['PRIVATE_KEY'] = realpath(__DIR__ . '/../../support/ssl/driver.key');
        $env['PASSPHRASE']  = 'php-driver';
        // Don't reset the LANG variable on HHVM, because it breaks HHVM itself
        if (!defined('HHVM_VERSION')) {
            $env['LANG'] = 'en'; // Ensures that the default language is en, whatever the OS locale is.
        }
        $this->process->setEnv($env);
        $this->process->start();
        $this->process->wait();
    }

    /**
     * @Then /^its output should contain:$/
     */
    public function itsOutputShouldContain(PyStringNode $string)
    {
        PHPUnit_Framework_Assert::assertContains((string) $string, $this->getOutput());
    }

    private function getOutput()
    {
        $output = $this->process->getErrorOutput() . $this->process->getOutput();
        // Normalize the line endings in the output
        if ("\n" !== PHP_EOL) {
            $output = str_replace(PHP_EOL, "\n", $output);
        }
        // Replace wrong warning message of HHVM
        $output = str_replace('Notice: Undefined index: ', 'Notice: Undefined offset: ', $output);
        return trim(preg_replace("/ +$/m", '', $output));
    }

    private function createFile($filename, $content)
    {
        $path = dirname($filename);
        if (!is_dir($path)) {
            mkdir($path, 0777, true);
        }
        $autoload = realpath(__DIR__.'/../../vendor/autoload.php');
        $content  = preg_replace('/\<\?php/', "<?php include '$autoload';", $content, 1);
        file_put_contents($filename, $content);
    }

    private static function clearDirectory($path)
    {
        $files = scandir($path);
        array_shift($files);
        array_shift($files);
        foreach ($files as $file) {
            $file = $path . DIRECTORY_SEPARATOR . $file;
            if (is_dir($file)) {
                self::clearDirectory($file);
            } else {
                unlink($file);
            }
        }
        rmdir($path);
    }
}
