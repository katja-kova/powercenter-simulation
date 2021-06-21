# VS21-Kovalchuk-Daebler

## About The Project

As part of the distributed systems course, an application from the field "power generation/powerplants" is to 
be developed. The following technologies are to be used: Sockets, RPC (Apache Thrift, gRPC / Protobuf) and Message-Oriented Middleware (MQTT).

![System Design](./docs/system.JPG)

## Built With

- C++
- Docker
- Make
- gRPC/Protobuf

## Getting Started

1. Clone the repo
    ```sh
    git@code.fbi.h-da.de:istkakova/vs21-kovalchuk-daebler.git
    ```
2. Build the project
    ```sh
    ./build.sh
    ```
3. Start docker containers
   ```sh
   docker-compose up
    ```
4. Check the data in the browser under `localhost/history`

### License

Distributed under the GNU General Public License. See `LICENSE` for more information.
### Team
Kateryna Kovalchuk **762835**    
Fynn Daebler **760412**
