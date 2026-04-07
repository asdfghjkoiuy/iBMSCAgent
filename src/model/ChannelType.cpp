#include "model/ChannelType.h"

namespace Model {

ChannelType classifyChannel(int ch) {
    switch (ch) {
        case 1:  return ChannelType::BGM;
        case 3:  return ChannelType::BPM;
        case 4:  return ChannelType::BGA;
        case 5:  return ChannelType::Layer;
        case 6:
        case 7:  return ChannelType::BGAExtended;
        case 8:  return ChannelType::BpmExtended;
        case 9:  return ChannelType::Stop;
        default: break;
    }
    // All channel identifiers are base-36 decoded. Common ranges:
    // P1 normal notes: "11"-"19" = 1*36+1..1*36+9 = 37-45
    if (ch >= 37 && ch <= 45) return ChannelType::NoteNormal;
    // P2 normal notes: "21"-"29" = 2*36+1..2*36+9 = 73-81
    if (ch >= 73 && ch <= 81) return ChannelType::NoteNormal;
    // P1 hidden notes: "31"-"39" = 3*36+1..3*36+9 = 109-117
    if (ch >= 109 && ch <= 117) return ChannelType::NoteHidden;
    // P2 hidden notes: "41"-"49" = 4*36+1..4*36+9 = 145-153
    if (ch >= 145 && ch <= 153) return ChannelType::NoteHidden;
    // P1 long notes: "51"-"59" = 5*36+1..5*36+9 = 181-189
    if (ch >= 181 && ch <= 189) return ChannelType::NoteLong;
    // P2 long notes: "61"-"69" = 6*36+1..6*36+9 = 217-225
    if (ch >= 217 && ch <= 225) return ChannelType::NoteLong;
    // Landmine notes: "D1"-"D9" = 13*36+1..13*36+9 = 469-477
    //                 "E1"-"E9" = 14*36+1..14*36+9 = 505-513
    if ((ch >= 469 && ch <= 477) || (ch >= 505 && ch <= 513)) return ChannelType::NoteMine;
    // Legacy iBMSC B-group dynamic lanes are represented internally as 101-132.
    if (ch >= 101 && ch <= 132) return ChannelType::BGM;
    return ChannelType::Unknown;
}

bool isPlayerNoteChannel(ChannelType t) {
    return t == ChannelType::NoteNormal || t == ChannelType::NoteLong;
}

bool isVisibleChannel(ChannelType t) {
    return t == ChannelType::NoteNormal
        || t == ChannelType::NoteLong
        || t == ChannelType::NoteHidden
        || t == ChannelType::NoteMine
        || t == ChannelType::BPM
        || t == ChannelType::BpmExtended
        || t == ChannelType::Stop;
}

} // namespace Model
