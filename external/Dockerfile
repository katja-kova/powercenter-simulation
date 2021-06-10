FROM koda_baseimg
ENV REQUEST_TARGET_IP="172.20.0.60";
ENV REQUEST_TARGET_PORT="50001";
ENV RESPONSE_PAGE_SIZE="10000";
COPY ./src/ /app/cpp/src
WORKDIR /app/cpp/src/cmake/build
RUN cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR -Wno-dev ../..; make -j 16
#RUN rm -f *.cc *.h Makefile CMakeCache.txt *.cmake
CMD ./external

