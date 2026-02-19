# Source Bug Audit

## 1) 치명적: 재귀 호출로 인한 스택 오버플로우 가능
- 파일: `Dev/CameraWrapImpl.cpp`
- 내용: `EncoderParamsbySpeed`, `EncoderParamsbyPulseInterval`, `EncoderParamsbyYScale`가 전역 SDK 함수를 의도한 것으로 보이나, 동일/유사한 이름을 멤버 스코프에서 unqualified call로 호출하고 있어 자기 자신을 재귀 호출할 위험이 있습니다.
- 조치: 전역 함수 호출임을 명시하도록 `::GetEncoderParams...` 형태로 수정했습니다.

## 2) 잠재적 크래시: `m_camera` 널 체크 없는 직접 접근
- 파일: `Dev/CameraWrapImpl.cpp`
- 내용: `GetMaxFram`, `GetCaptureMode`, `SetExpo` 등 다수 메서드가 `m_camera`를 바로 사용합니다. 카메라 연결 실패/해제 직후 호출되면 null dereference 가능성이 있습니다.
- 권장: 공통 가드 함수(예: `if (!m_camera) return false/0;`) 도입 또는 연결 상태 기반 방어 코드 추가.

## 3) 리소스 누수 가능성: 카메라 핸들 파기 코드 부재 의심
- 파일: `Dev/CameraWrapImpl.cpp`
- 내용: 생성자에서 `SgCreateCamera()`를 호출하지만, 소멸자에서 대응되는 destroy API 호출이 보이지 않습니다.
- 권장: SDK에서 제공하는 destroy/free API(`SgDestroyCamera` 계열) 확인 후 소멸자에 추가.

## 4) 안정성 이슈: 실패 시 초기화되지 않은 값 반환 가능
- 파일: `Dev/CameraWrapImpl.cpp`
- 내용: `GetPointCloudCallBackNum`는 SDK 호출 실패 여부를 확인하지 않고 지역 변수 `iNum`을 반환합니다.
- 권장: 기본값 초기화(`int iNum = 0;`) 및 SDK 반환코드 체크 추가.

## 5) 안전하지 않은 문자열 포맷팅 API 사용
- 파일: `UIExt/StripInfoCtrl.cpp`, `UIExt/ObjectInfoCtrlEx.cpp`
- 내용: `wsprintfW` 사용은 버퍼 길이 인자가 없어 향후 확장 시 오버플로우 리스크가 있습니다.
- 권장: `StringCchPrintfW` 등 길이 제한 API로 교체.
