# AppleGameHelper

**AppleGameHelper**는 간단한 화면 캡처 기반 게임 자동화 도구입니다.  
특정 영역을 캡처하고, OCR(문자인식)을 통해 그리드 숫자를 인식한 뒤,  
자동으로 제거 경로를 계산하여 클릭을 자동화합니다.

## ✨ 주요 기능

- **영역 캡처**  
  사용자가 지정한 화면 영역을 캡처하고, 내부 그리드를 인식합니다.

- **OCR 처리**  
  캡처된 영역의 숫자를 Tesseract OCR을 이용해 빠르게 인식합니다.

- **자동화 실행**  
  인식된 숫자 그리드 기반으로 최적의 제거 경로를 계산하고, 마우스 자동 클릭을 수행합니다.

- **진행 중 중단 및 재개**  
  자동화를 중간에 중단할 수 있으며, 진행상황을 저장하고 중단 이후 다시 이어서 자동화를 수행할 수 있습니다.

- **다양한 모드 지원**  
  - **일반 모드 (Normal Mode)**: 기본 OCR 및 자동화
  - **점수 모드 (Score Mode)**: 특정 점수 기준 자동화 진행
  - **데이터 수집 모드 (Data Collection Mode)**: 캡처 이미지 저장

- **핫키 지원**  
  자동화 중 전역 단축키를 사용하여 작업 중단이 가능합니다.

## 📂 프로젝트 구조
AppleGameHelper/
├── src/
│       ├── controllers/     # AppController, CaptureManager, AutomationManager 등
│       │        ├── strategies/      # NormalMode, ScoreMode, DataCollectionMode (모드 파일)
│       │
│       ├── core/             # ImageProcessor, OCRManager 등 핵심 로직
│       ├── shared/          # HotKeyManager, OverlayHelper 등 공유 로직
│       ├── tabs/             # MainTab UI
│       ├── main/            # 실행 진입점
│       ├─ resources/         # 아이콘, 스타일 파일 등
│
├── CMakeLists.txt       # CMake 빌드 설정
└── README.md         # 프로젝트 소개 파일
└── docs/images 	    # 예시 이미지

⚙️ 개발 환경
C++20
Qt 6
OpenCV (이미지 처리)
Tesseract OCR (문자 인식)

🚀 빌드 방법
필수 라이브러리 설치
Qt 6
OpenCV 4.x
Tesseract 5.x

CMake 사용

cd AppleGameHelper
mkdir build
cd build
cmake ..
cmake --build .

또는 VisualStudio에서 폴더 열기를 통해 CMake 프로젝트로 빌드 가능합니다.


📸 사용 방법
프로그램을 실행합니다.

1. "Capture" 버튼을 눌러 게임 화면의 영역을 설정합니다.

2. "OCR" 버튼을 눌러 숫자 인식을 수행합니다.

3. "Auto" 버튼을 누르면 자동으로 제거 작업이 시작됩니다.

3. 중간에 핫키(Ctrl+Shift+S 등)로 중단할 수 있으며, 재개도 가능합니다.

4. 새 게임을 시작할 경우 "Capture"를 다시 눌러 새로운 영역을 설정하세요.


주요 이슈
OCR 정확도 100% 보장되지 않음.

게임 링크
https://www.gamesaien.com/game/fruit_box_a/

📜 라이선스
본 프로젝트는 개인 포트폴리오용으로 개발되었습니다.
