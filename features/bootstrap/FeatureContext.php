<?php

use Behat\Behat\Context\Context;
use Behat\Behat\Context\SnippetAcceptingContext;
use Behat\Gherkin\Node\PyStringNode;
use Behat\Gherkin\Node\TableNode;
use Symfony\Component\Process\PhpExecutableFinder;
use Symfony\Component\Process\Process;
use Behat\Behat\Tester\Exception\PendingException;

/**
 * Defines application features from the specific context.
 */
class FeatureContext implements Context, SnippetAcceptingContext
{
    /**
     * Initializes context.
     *
     * Every scenario gets its own context instance.
     * You can also pass arbitrary arguments to the
     * context constructor through behat.yml.
     */
    public function __construct()
    {
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
    * @Given /^the following schema:$/
    */
    public function theFollowingSchema(PyStringNode $string)
    {
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
