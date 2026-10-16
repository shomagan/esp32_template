FROM espressif/idf:v6.0

RUN dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get install -y --no-install-recommends gcc-multilib libbsd-dev libbsd-dev:i386 \
    && rm -rf /var/lib/apt/lists/*

RUN bash -c "source $IDF_PATH/export.sh > /dev/null && python -m pip install --no-cache-dir pytest gitpython"
