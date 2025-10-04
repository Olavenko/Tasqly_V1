# ADR 0008: Docker Dev Container — Development Environment Baseline (Phase 1)

- **Status**: Planned
- **Date**: 2025-09-01
- **Owners**: Tasqly Core Infra (DevOps & Build Systems)
- **Related**: Phase 0 — Slice 0 (Project Initialization & Testing Foundation); CI matrix; `building.md`; PlantUML workflows
- **Tags**: tooling, build, reproducibility, onboarding

---

## Context
Tasqly targets cross‑platform development with Qt Quick (QML) + C++20 and PostgreSQL. Local developer environments currently rely on host‑installed toolchains (CMake, Ninja, compilers, Qt SDK, Java/Graphviz for PlantUML). This causes variability across OSes and complicates onboarding.

Phase 0 يتجنب إضافة Docker لتبسيط الأساس. لكن لضمان reproducible builds وتجربة موحدة، سنقدم **dev container** أساسي باستخدام Docker في **Phase 1**.

---

## Decision
اعتماد **Docker-based development container** كبيئة اختيارية ومرجعية بدءًا من **Phase 1**.

- **Base OS**: Ubuntu LTS (مثال: 24.04).
- **Tools**: CMake, Ninja, GCC/Clang, Python3, Git, Java + PlantUML, Graphviz, clang-tidy/format, cppcheck، و **Qt 6 (حد أدنى)** للتجميع وتشغيل الاختبارات.
- **User model**: تشغيل كمستخدم غير root مع مطابقة UID/GID.
- **Workspace**: bind mount للمشروع في `/work`.
- **No secrets** داخل الصورة.
- يبقى الحاوية **اختيارية** للمطورين، لكنها مرجع لحل مشاكل التوافق وعلى متن الفريق.

---

## Consequences
### إيجابيات
- قابلية إعادة الإنتاج وتقليل "يعمل عندي فقط".
- Onboarding أسرع وحزم أدوات موحدة.
- تقارب مع بيئة CI وإمكانية تشغيل linters/tests بنفس الإعدادات.

### سلبيات/مقايضات
- حجم الصورة وزمن bootstrap الأولي.
- متطلبات Docker Desktop/WSL2 على Windows.
- صيانة مسارين (native + container).

---

## Constraints & Performance Bars (Phase 1)
- **Bootstrap ≤ 20 دقيقة** على اتصال نموذجي.
- **Image size ≤ 6GB** (مفضل 4–5GB).
- **Build parity**: تجميع `TasqlyApp` + `TasqlyTests` وتشغيل `ctest` بنجاح.
- `uml-export.sh` و static analysis تعمل داخل الحاوية.

---

## Interfaces & DX
**ملفات جديدة (Phase 1):**
- `tools/docker/Dockerfile.dev`
- `tools/docker/.dockerignore`
- `tools/devshell.sh`
- (اختياري) `.devcontainer/devcontainer.json`

**أوامر:**
- `./tools/devshell.sh` لفتح dev shell
- (اختياري) `make devshell` أو preset

**التوثيق:**
- تحديث `building.md` لإضافة مسار الحاوية.

---

## Alternatives
- بدون حاوية (native only).
- Nix/Flakes.
- vcpkg/Conan فقط.
- VMs/WSL distro أثقل.

---

## Rollout Plan (Phase 1)
1. إنشاء `Dockerfile.dev` بقاعدة مثبتة ومثبتات مُقيدة.
2. سكربت `devshell.sh` مع UID/GID bind mounts.
3. تحديث `building.md` (quick start).
4. (اختياري) `.devcontainer`.
5. Smoke test: build + ctest + UML export.
6. قياس الحجم والوقت وتحسين الطبقات.

**Acceptance Criteria**
- بناء `TasqlyApp` و `TasqlyTests` داخل الحاوية.
- نجاح `ctest`.
- نجاح تصدير PlantUML.
- تشغيل linters/formatters/cppcheck.

---

## Open Questions
- طريقة تثبيت Qt داخل الحاوية (apt vs aqtinstall vs installer) والقيود الترخيصية/الحجم.
- المترجم الافتراضي داخل الحاوية (clang أم gcc) لتحقيق parity مع CI.
- نشر صورة مُسبقة في registry أم بناء محليًا؟

---

## Decision Log
- 2025-09-01: ADR 0007 مضافة كـ "Planned" في Phase 0؛ التنفيذ يبدأ في Phase 1.
