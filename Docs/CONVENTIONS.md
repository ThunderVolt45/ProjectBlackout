# 프로젝트 블랙아웃 — 개발 컨벤션 및 협업 가이드

> **원본**: [Notion — 개발 컨벤션 및 협업 가이드](https://www.notion.so/3428a69198078050b440d40ec168ed58)
> **최종 동기화**: 2026-04-15

본 문서는 '프로젝트 블랙아웃'의 팀원들이 원활하게 협업하고, 프로젝트 폴더와 코드베이스를 청결하게 유지하기 위한 필수 약속(Convention)을 정의합니다.

---

## 목차

1. [에셋 네이밍 규칙](#1-에셋-네이밍-규칙-asset-naming-convention)
2. [언리얼 C++ 코딩 표준](#2-언리얼-c-코딩-표준-c-coding-standard)
3. [폴더 구조 가이드](#3-폴더-구조-가이드-directory-structure)
4. [Git 브랜치 전략 및 커밋 메시지 규칙](#4-git-브랜치-전략-및-커밋-메시지-규칙)

---

## 1. 에셋 네이밍 규칙 (Asset Naming Convention)

언리얼 엔진 에디터 내에서 에셋을 생성할 때는 반드시 **접두사(Prefix)**를 붙여 에셋의 성격을 명확히 합니다.

| 에셋 유형 (Asset Type)       | 접두사 (Prefix) | 예시 (Example)      |
| ---------------------------- | --------------- | ------------------- |
| Blueprint Class              | `BP_`           | `BP_PlayerCharacter`|
| Skeletal Mesh                | `SKM_`          | `SKM_Ravager`       |
| Static Mesh                  | `SM_`           | `SM_PillarBase`     |
| Material                     | `M_`            | `M_RedMist`         |
| Material Instance            | `MI_`           | `MI_RedMist_Dark`   |
| Texture / UI Image           | `T_`            | `T_Crosshair`       |
| Widget Blueprint             | `WBP_`          | `WBP_PartyInfo`     |
| Sound Cue                    | `SC_`           | `SC_GunShot`        |
| Particle System (Niagara)    | `NS_`           | `NS_Explosion`      |
| Level / Map                  | `LV_`           | `LV_BossArena`      |

- 형식: `Prefix_Name_PostFix(선택)`
- 띄어쓰기는 사용하지 않으며, 단어의 구분은 **PascalCase** 또는 `_`를 사용합니다.

---

## 2. 언리얼 C++ 코딩 표준 (C++ Coding Standard)

에픽게임즈 코딩 표준을 기반으로 프로젝트의 일관성과 엔진 편의성을 높입니다.

### 클래스 및 구조체 명명 규칙

| 접두사 | 대상                      | 예시                        |
| ------ | ------------------------- | --------------------------- |
| `A`    | Actor 상속                | `ABlackoutCharacter`, `ABOWeapon` |
| `U`    | UObject / 컴포넌트 상속   | `UBOCombatComponent`        |
| `F`    | 구조체 (Struct)           | `FBlackoutWeaponStat`       |
| `E`    | 열거형 (Enum)             | `EBOPlayerState`            |
| `I`    | 인터페이스 (Interface)    | `IBlackoutInteractable`     |

### 프로젝트 고유 접두어 규칙

- 모든 커스텀 클래스는 언리얼 타입 접두사(`A`, `U`, `F` 등) 뒤에 **프로젝트 명칭**을 붙입니다.
- **기본**: `Blackout` (완전형) 우선 사용
- **예외**: 클래스명이 **35자 이상**이 되는 경우 약어 `BO`로 대체 가능
  - 완전형 예시: `ABlackoutGameMode`, `ABlackoutLobbyGameMode`, `ABlackoutPlayerCharacter`
  - 약어형 예시: `ABOCombatComponent`, `ABOPoolSubsystem`, `ABOBossCharacter`

### 변수 명명 규칙

- `bool` 타입 변수에는 반드시 `b` 접두사를 붙입니다. (예: `bIsDowned`, `bHasRelic`)
- 모든 변수 및 함수는 **PascalCase**를 사용합니다.

### 블루프린트 변수 (UPROPERTY) 노출 규칙

- 블루프린트 에디터로 노출하는 모든 변수는 **Category 대분류를 `"Blackout"`으로 설정**합니다.
- 예시: `UPROPERTY(EditAnywhere, Category = "Blackout|Combat")`

### 접근 제어자

- 멤버 변수는 `private` 또는 `protected`로 캡슐화하고, 필요한 경우에만 `public` Getter/Setter를 제공합니다.

---

## 3. 폴더 구조 가이드 (Directory Structure)

### Content 디렉토리 (에셋)

`_BP` 폴더에 **블루프린트 클래스만** 모아두며, 외부 에셋은 `Assets/`에 격리 보관합니다.

```
Content/
 ┣ _BP/            ← 프로젝트의 모든 블루프린트를 모아두는 최상위 폴더
 ┃ ┣ Characters/   (플레이어 및 몬스터 블루프린트)
 ┃ ┣ Weapons/      (무기 및 발사체 블루프린트)
 ┃ ┣ UI/           (위젯 블루프린트 등)
 ┃ ┗ Core/         (GameMode, GameState 등 프레임워크 필수 블루프린트)
 ┣ Assets/         ← 마켓플레이스 구매/서드파티 외부 에셋 원본 보관 폴더
 ┣ Characters/     (메쉬, 애니메이션 등 캐릭터 비주얼 에셋)
 ┣ Weapons/        (무기 모델링, 파티클 이펙트 등)
 ┣ Maps/           (레벨 에셋)
 ┗ Materials/      (글로벌 재질 등)
```

### Source 디렉토리 (C++ 코드)

```
Source/ProjectBlackout/
 ┣ ProjectBlackout.h / .cpp              ← 모듈 진입점
 ┣ ProjectBlackoutCharacter.h / .cpp     ← 기본 캐릭터
 ┣ ProjectBlackoutGameMode.h / .cpp      ← 기본 게임모드
 ┗ ProjectBlackoutPlayerController.h/.cpp
```

> **🚨 Variant_ 디렉토리 배제 규칙**
> `Variant_Combat/`, `Variant_Platforming/`, `Variant_SideScrolling/` 세 디렉토리는 **언리얼 엔진 템플릿이 자동 생성한 샘플 코드**입니다. 이번 프로젝트에서는 해당 코드를 일절 사용하지 않습니다.
> - **기존 파일을 참고하거나 수정하지 마세요.**
> - **새 기능을 이 디렉토리 하위에 추가하지 마세요.**
> - 프로젝트 신규 코드는 반드시 모듈 루트(`Source/ProjectBlackout/`) 아래에 목적에 맞는 폴더를 새로 만들어 작성합니다.

---

## 4. Git 브랜치 전략 및 커밋 메시지 규칙

### 브랜치 구조 (Branch Flow)

| 브랜치           | 용도                                                      |
| ---------------- | --------------------------------------------------------- |
| `main`           | 출시 가능한 안정적인 최종 버전. **절대 직접 커밋 금지**.  |
| `develop`        | 개발 중인 최신 코드가 모이는 곳.                          |
| `feature/[기능명]` | 개인 작업 브랜치. `develop`에서 분기, PR 후 삭제.         |

- 예시: `feature/boss-ai`, `feature/weapon-mod`

### 커밋 메시지 (Commit Message) 양식

```
[태그]: [작업 내용 요약 (50자 이내)]

[상세 내용 (선택)]
- 무슨 목적을 가지고 어떤 구조로 변경했는지
- 팀원들이 공유해야 할 주의사항이나 참고 문서

[이슈 번호] (예: Resolve #12)
```

#### 태그(Tag) 목록

| 태그       | 용도                                           | 예시                                                  |
| ---------- | ---------------------------------------------- | ----------------------------------------------------- |
| `Feat`     | 새로운 기능 추가                               | `Feat: 무기 스킬 게이지 충전 로직 구현`               |
| `Fix`      | 버그 및 오류 수정                              | `Fix: 멀티플레이 시 캐릭터 다운 애니메이션 미작동 수정`|
| `Design`   | UI/UX 구조 레이아웃 배치 및 시각적 변경        |                                                       |
| `Refactor` | 최적화, 코드/블루프린트 구조 개선 (기능 변화 없음) |                                                   |
| `Docs`     | 기획서, TDD 가이드 등 문서 작성/수정           |                                                       |
| `Chore`    | 빌드 패키징, 플러그인 갱신, 에셋 네이밍 수정 등 |                                                      |

### Pull Request (PR) 가이드

PR은 기능 브랜치를 `develop`에 반영하기 위한 필수 안전장치이자 동료 리뷰 과정입니다.

#### PR 템플릿 양식

```markdown
## 📝 작업 내용 요약
- [ ] 작업 항목 1
- [ ] 작업 항목 2

## 📸 스크린샷 (선택)
- (블루프린트 구성 캡처나 게임플레이 스크린샷 첨부)

## 🤔 의논 및 리뷰 요청 사항
- 리뷰어에게 집중적으로 봐달라고 요청할 부분

## 🔗 관련 이슈 (선택)
- Resolved #이슈번호
```

#### PR 규칙

- 최소 **1명의 팀원(Reviewer)**의 Approve를 받아야 합니다.
- 최종 Merge 버튼 클릭은 오직 **Repo 소유자(팀장)**만 수행할 수 있습니다.
