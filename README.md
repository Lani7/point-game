# point-game
포인트 게임은 C언어로 구현한 간단한 콘솔 게임입니다.  
노란색 먹이(동그라미)를 먹으면 포인트를 얻고 장애물(다이아몬드)를 만나면 게임이 오버됩니다.

## 동기
인터넷 강의를 통해서 C언어를 배우고 난 뒤 해당 언어를 사용하여 미니 프로젝트를 진행해보고 싶어 만들게 되었습니다.

## 기획
1. 게임을 시작하기 전 사용자의 닉네임을 기입받는다.
2. 플레이어는 키보드키를 이용해 상하좌우로 움직일 수 있다.
3. 노란색 먹이(동그라미)를 먹으면 포인트가 증가한다.
4. 빨간색 장애물(다이아몬드)를 만나면 게임 종료된다.
5. 빨간색 장애물(다이아몬드)는 좌우로 움직인다.
6. 노란색 동그라미를 다 먹으면 Stage가 올라가며 장애물(다이아몬드)의 속도와 개수가 증가한다.
7. 게임 종료 후 사용자의 점수와 상위 랭킹 5위의 닉네임과 점수를 함께 보여준다.
8. 사용자가 다시 하기를 선택할 수 있도록 한다.

## 개발 환경
* MacOS
* Visual Studio Code

## 실행 방법
<ncurses.h>를 include하고 있기 때문에 라이브러리를 사용하기 위해 컴파일 시 -lncurses 옵션을 주셔야 합니다.

    gcc main.c -lncurses -o main
    ./main
    

## 시연 영상 및 후기 
https://lani-develop.tistory.com/31

## 참고 자료
https://codemate.kr/project/C%EC%96%B8%EC%96%B4-%EB%A9%94%EC%9D%B4%ED%8A%B8-%EC%8B%AC%ED%99%94%ED%8E%B8-HUDkY14Q1u/10-2.-%EB%AF%B8%EB%A1%9C%EC%B0%BE%EA%B8%B0-for-mac

https://geundung.dev/14

https://huiyu.tistory.com/entry/C%EC%96%B8%EC%96%B4%EB%A1%9C-%EA%B2%8C%EC%9E%84%EB%A7%8C%EB%93%A4%EA%B8%B0-5-%EC%B2%AB%EB%B2%88%EC%A7%B8-%EA%B2%8C%EC%9E%843%EC%9B%80%EC%A7%81%EC%9D%B4%EB%8A%94-%EA%B3%A8%EB%8C%80-%EB%A7%8C%EB%93%A4%EA%B8%B0

