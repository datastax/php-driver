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
  ccm create -v binary:2.1.3 -b php-driver-cluster

  ccm updateconf --rt 1000 'read_request_timeout_in_ms: 1000'         \
    'write_request_timeout_in_ms: 1000' 'request_timeout_in_ms: 1000' \
    'phi_convict_threshold: 16' 'hinted_handoff_enabled: false'       \
    'dynamic_snitch_update_interval_in_ms: 1000'                      \
    'cas_contention_timeout_in_ms: 10000' 'file_cache_size_in_mb: 0'  \
    'native_transport_max_threads: 1' 'rpc_min_threads: 1'            \
    'rpc_max_threads: 1' 'concurrent_reads: 2' 'concurrent_writes: 2' \
    'concurrent_compactors: 1' 'compaction_throughput_mb_per_sec: 0'  \
    'key_cache_size_in_mb: 0' 'key_cache_save_period: 0'              \
    'memtable_flush_writers: 1' 'max_hints_delivery_threads: 1'

  ccm populate -n 1 -i 127.0.0.
  ccm start --verbose

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
