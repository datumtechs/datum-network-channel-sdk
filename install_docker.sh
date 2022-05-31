DOCKER_EXIST=$(ps -ef | grep dockerd | grep -v grep | wc -l)
if [ $DOCKER_EXIST -gt 0 ]; then
	exit 0
fi

# install docker
apt-get update

apt-get install -y \
	apt-transport-https \
	ca-certificates \
	curl \
	gnupg-agent \
	software-properties-common

curl -fsSL http://mirrors.aliyun.com/docker-ce/linux/ubuntu/gpg | sudo apt-key add -

add-apt-repository "deb [arch=amd64] http://mirrors.aliyun.com/docker-ce/linux/ubuntu $(lsb_release -cs) stable"

apt-get update
apt-get install -y docker-ce docker-ce-cli containerd.io

docker-compose --version
if [ $? != 0 ]; then
	# install docker-compose
	curl -L "https://github.com/docker/compose/releases/download/1.28.6/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
	chmod +x /usr/local/bin/docker-compose
	# test the installation
	docker-compose --version
fi
