# BattleGame Server
BattleGame 서버
* 2학년 겨울방학 프로젝트
* 스크린샷은 BattleGame Client(https://github.com/floweryclover/battlegame-client)참고

## 특징
* 클라이언트랑 마찬가지로 외부 라이브러리 없이 Winsock과 linux os 함수들 사용
* Windows/Linux 둘 모두에서 실행 가능
* 싱글스레드 모델
    * Accept, 송수신 모두 논블로킹 IO로 작동
    * 네트워크 IO 이외에도 게임 방 처리도 메인 스레드에서 같이 처리
        * main.cpp에서 Tick()을 호출하는 루프가 네트워크와 게임 로직의 모든 Tick()을 함께 호출하는 구조

## 한계점
* 코드 정리가 잘 안되어 있어서 Wsa.h/cpp처럼 굳이 별도 파일로까지 작성하지 않아도 될 코드가 있는 등 지저분한 점이 있음
    * 이외에도 사용하지 않는 코드(GameRoom의 OneVsOneRoom의 팀 기능이라던지)가 남아있는 경우가 있음
* 네트워크 IO 관련해서 비효율적이고 불안정(recv/send 호출부터 클라이언트 상태 관리 등등)