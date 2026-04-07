## ADDED Requirements

### Requirement: Keysound loading
The system SHALL load WAV resource files referenced in the BMS WAV table from disk into memory as decoded PCM. Supported formats SHALL include WAV (PCM, ADPCM), OGG Vorbis, and MP3.

#### Scenario: Load WAV file
- **WHEN** a BMS document with a #WAV01 entry pointing to "snare.wav" is loaded
- **THEN** slot 1 SHALL contain decoded PCM audio data ready for playback

#### Scenario: Missing file graceful handling
- **WHEN** a WAV slot references a file that does not exist on disk
- **THEN** the slot SHALL be marked as missing and playing it SHALL produce silence (no crash)

### Requirement: BMS chart-synchronized playback
The system SHALL play back the BMS chart by scheduling keysound events in timeline order and triggering each keysound at its computed absolute timestamp. Playback SHALL start from any measure position.

#### Scenario: Play from beginning
- **WHEN** the user presses Play
- **THEN** keysounds SHALL begin playing in sequence according to the chart's BGM channel and timing

#### Scenario: Play from measure N
- **WHEN** the user starts playback from measure 10
- **THEN** only notes at or after measure 10 SHALL be scheduled; notes before are skipped

#### Scenario: Stop playback
- **WHEN** the user presses Stop
- **THEN** all active keysounds SHALL stop immediately and the playback position SHALL reset or hold

### Requirement: Simultaneous polyphonic playback
The system SHALL support playing up to 64 simultaneous keysound instances without audio artifacts. Each sound plays to completion unless explicitly stopped.

#### Scenario: Dense chord playback
- **WHEN** 10 notes are triggered within the same millisecond
- **THEN** all 10 sounds SHALL play simultaneously without distortion

### Requirement: Playback position reporting
The audio engine SHALL expose the current playback position in seconds to the UI layer at least every 16ms (one frame at 60fps) so the chart canvas cursor can be updated.

#### Scenario: Position updates
- **WHEN** playback is active
- **THEN** a position signal/callback SHALL be emitted at ≥60Hz with the current time in seconds

### Requirement: Audio device selection
The user SHALL be able to select the audio output device from application settings. The engine SHALL use the system default device if none is configured.

#### Scenario: Default device
- **WHEN** no device is configured
- **THEN** playback SHALL use the OS default audio output device
