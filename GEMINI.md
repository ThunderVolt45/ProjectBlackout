# Project Blackout — Gemini CLI Instructions

> Gemini CLI 전용 지침입니다. 공통 규칙은 반드시 아래 파일들을 먼저 읽으세요.

## 필수 참조 문서

- **공통 AI Agent 지침**: [`AGENTS.md`](AGENTS.md)
- **상세 개발 컨벤션**: [`Docs/CONVENTIONS.md`](Docs/CONVENTIONS.md)

## Gemini CLI 전용 설정

### 작업 시작 전 체크리스트

1. `AGENTS.md`를 읽어 프로젝트 개요와 소스 구조를 파악하세요.
2. `Docs/CONVENTIONS.md`를 읽어 네이밍, 코딩, Git 컨벤션을 숙지하세요.
3. 현재 브랜치가 `feature/` 브랜치인지 확인하세요. 아니라면 새로 생성하세요.

### 코드 생성 시 주의사항

- Unreal Engine 5 C++ 코드를 생성할 때는 `#include` 경로가 모듈 구조(`ProjectBlackout/...`)와 일치하는지 확인하세요.
- `.generated.h` 인클루드를 절대 빠뜨리지 마세요.
- `UCLASS()`, `USTRUCT()`, `UENUM()` 매크로 사용 시 적절한 Specifier를 포함하세요.
- 헤더 파일에는 `#pragma once`를 사용하세요.

### 커밋 워크플로우

- 커밋 메시지는 `Docs/CONVENTIONS.md`의 태그 형식을 따르세요.
- 커밋 전 변경 파일 목록을 확인하고, `.uasset` / `.umap` 바이너리 파일이 포함되어 있다면 주의하세요.

### 금지 사항

- `main` 또는 `develop` 브랜치에 직접 커밋하지 마세요.
- `Content/Assets/` 하위 서드파티 에셋을 수정하지 마세요.
- `.uproject`, `.uplugin` 파일을 임의로 수정하지 마세요.
