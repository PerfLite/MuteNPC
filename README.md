# MuteNPC (SKSE64 Plugin)

<p align="center">
  <a href="#english"><b>English</b></a> | <a href="#русский"><b>Русский</b></a>
</p>

---

<a name="english"></a>
## English

A native C++ SKSE64 plugin powered by [CommonLibSSE-ng](https://github.com/CharmedBaryon/CommonLibSSE-ng) for selectively and completely muting annoying NPCs in **Skyrim Special Edition / Anniversary Edition / VR**.

### 🎯 Overview

Tired of Heimskr shouting all day in Whiterun? Annoyed by Nazeem's condescending remarks? Or any other NPC constantly spamming voice lines?
**MuteNPC** lets you silence any NPC with a single click. Muted characters remain completely silent everywhere: in the open world, during preaching scenes, in direct dialogue ('E' key), and when saying goodbye.

### ✨ Features

1. **One-Click Target Muting:**
   * Aim your crosshair at any NPC.
   * Open **SKSE Menu Framework** ➔ **MuteNPC / Settings**.
   * Click **"Mute this target"** — the NPC immediately goes silent, even mid-sentence.
   * Click **"Unmute this target"** if you ever want their voice back.

2. **Complete Silence:**
   * Voice audio descriptors are cleared (`voiceSound = nullptr`, `voice = ""`).
   * Silences all lines: ambient chatter, scene preachings, greetings, direct dialogue menu choices, and goodbyes.

3. **Subtitle Suppression:**
   * `[x] Hide subtitles for muted dialogue` — completely removes floating subtitles for muted NPC lines so your screen stays clean.

4. **Clean In-Game UI:**
   * Seamless integration with [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352).
   * Muted NPC list with convenient `[Remove]` buttons placed on the left for easy access.
   * One-click clear list and reset options.
   * No technical IDs, clutter, or hardcoded actors.

5. **Multi-Language Support (JSON Localization):**
   * Automatically detects game language (e.g. English, Russian) from your game settings.
   * Translations are loaded via simple JSON files in `SKSE/Plugins/`:
     * `MuteNPCStrings_EN.json` (English)
     * `MuteNPCStrings_RU.json` (Russian)
   * Anyone can easily add their own language: just create `MuteNPCStrings_<LANG>.json` (e.g., `MuteNPCStrings_DE.json`, `MuteNPCStrings_FR.json`, `MuteNPCStrings_ES.json`).

### 🔧 Requirements

* **Skyrim Special Edition** (1.5.97) / **Anniversary Edition** (1.6.x) / **Skyrim VR**
* [SKSE64](https://skse.silverlock.org/)
* [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352)
* [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

### 📦 Installation

Install with your favorite mod manager (**Mod Organizer 2** or **Vortex**).

Package structure:
```
SKSE/
  Plugins/
    MuteNPC.dll
    MuteNPCStrings.json
    MuteNPCStrings_EN.json
    MuteNPCStrings_RU.json
```

### 🛠️ Building from Source

Built using [xmake](https://xmake.io/) and [CommonLibSSE-ng](https://github.com/CharmedBaryon/CommonLibSSE-ng):

```bash
xmake config -m release
xmake
```

Output binary: `build/windows/x64/release/MuteNPC.dll`.

---

<a name="русский"></a>
## Русский

Нативный SKSE64-плагин на C++ (на базе [CommonLibSSE-ng](https://github.com/CharmedBaryon/CommonLibSSE-ng)) для выборочного и полного заглушения надоедливых NPC в **Skyrim Special Edition / Anniversary Edition / VR**.

### 🎯 Описание

Надоел вечно кричащий Хеймскр в Вайтране? Раздражает надменный Назим? Или любой другой персонаж, спамящий фразами?
**MuteNPC** позволяет заглушить любого NPC в игре одним нажатием кнопки. Заглушённый персонаж затихает полностью: и в открытом мире / сценах, и при личном диалоге через клавишу «E», и при прощании.

### ✨ Возможности

1. **Мгновенное добавление в список заглушённых:**
   * Наведите прицел на любого NPC в игре.
   * Откройте меню **SKSE Menu Framework** ➔ **MuteNPC / Настройки**.
   * Нажмите **«Заглушить эту цель»** — персонаж мгновенно замолкает (даже если говорил прямо в этот момент).
   * Если захотите вернуть голос — нажмите **«Вернуть голос этой цели»**.

2. **Полная тишина:**
   * Звуковая дорожка реплики заглушённого NPC полностью обнуляется (`voiceSound = nullptr`, `voice = ""`).
   * Работает везде: спонтанные реплики, сцены проповедей, приветствия, ветки личного диалога и прощания.

3. **Опция скрытия субтитров:**
   * `[x] Скрывать субтитры заглушенных фраз` — убирает всплывающий текст фраз заглушённых персонажей, чтобы не засорять экран.

4. **Удобное меню в игре:**
   * Интеграция с [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352).
   * Список заглушённых персонажей с кнопками удаления `[Удалить]` слева от имени.
   * Кнопка быстрой очистки всего списка и сброса настроек.
   * Никакого мусора, лишних технических ID и загромождения интерфейса.

5. **Мультиязычность и перевод через JSON:**
   * Мод автоматически определяет язык игры (русский или английский).
   * Переводы хранятся в формате JSON в папке `SKSE/Plugins/`:
     * `MuteNPCStrings_RU.json` (Русский)
     * `MuteNPCStrings_EN.json` (Английский)
   * Любой пользователь или локализатор может легко сделать свой перевод: достаточно создать файл `MuteNPCStrings_<ЯЗЫК>.json` (например, `MuteNPCStrings_DE.json`, `MuteNPCStrings_FR.json` и т.д.).

### 🔧 Требования

* **Skyrim Special Edition** (1.5.97) / **Anniversary Edition** (1.6.x) / **Skyrim VR**
* [SKSE64](https://skse.silverlock.org/)
* [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352)
* [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

### 📦 Установка

Установите архив с помощью любого мод-менеджера (**Mod Organizer 2** или **Vortex**).

Структура файлов:
```
SKSE/
  Plugins/
    MuteNPC.dll
    MuteNPCStrings.json
    MuteNPCStrings_EN.json
    MuteNPCStrings_RU.json
```

### 🛠️ Сборка из исходников

Проект собирается с помощью [xmake](https://xmake.io/) и [CommonLibSSE-ng](https://github.com/CharmedBaryon/CommonLibSSE-ng):

```bash
xmake config -m release
xmake
```

Собранная библиотека появится в `build/windows/x64/release/MuteNPC.dll`.
