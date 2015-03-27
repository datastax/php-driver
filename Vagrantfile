Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/trusty"
  config.vm.box_url = "http://cloud-images.ubuntu.com/vagrant/trusty/current/trusty-server-cloudimg-i386-vagrant-disk1.box"
  config.vm.synced_folder ".", "/usr/local/src/php-driver"

  config.vm.provider "virtualbox" do |v|
    v.memory = 2048
    v.cpus   = 2
  end

  config.vm.provision "shell", inline: <<-SHELL
  echo "Installing all necessary packages"
  sudo apt-get update
  sudo apt-get install -y g++ make cmake libuv-dev libssl-dev libgmp-dev php5 php5-dev
  sudo apt-get install -y python-pip default-jdk
  sudo apt-get install -y git valgrind

  sudo su - vagrant

  sudo pip install virtualenv
  virtualenv venv
  source venv/bin/activate
  pip install ccm

  pushd /usr/local/src/php-driver/lib/cpp-driver
  cmake .
  make
  sudo make install
  popd
  pushd /usr/local/src/php-driver/ext
  phpize
  ./configure --with-cassandra
  make
  sudo make install
  sudo sh -c 'echo "extension=cassandra.so" > /etc/php5/cli/conf.d/100-cassandra.ini'
  popd
  pushd /usr/local/src/php-driver/
  curl -sS https://getcomposer.org/installer | php
  php composer.phar install
  popd
  SHELL
end
