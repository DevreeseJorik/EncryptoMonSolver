#pragma once

#include <string>
#include <unordered_map>

enum Items {
    None,
    MasterBall,
    UltraBall,
    GreatBall,
    PokeBall,
    SafariBall,
    NetBall,
    DiveBall,
    NestBall,
    RepeatBall,
    TimerBall,
    LuxuryBall,
    PremierBall,
    DuskBall,
    HealBall,
    QuickBall,
    CherishBall,
    Potion,
    Antidote,
    BurnHeal,
    IceHeal,
    Awakening,
    ParlyzHeal,
    FullRestore,
    MaxPotion,
    HyperPotion,
    SuperPotion,
    FullHeal,
    Revive,
    MaxRevive,
    FreshWater,
    SodaPop,
    Lemonade,
    MoomooMilk,
    EnergyPowder,
    EnergyRoot,
    HealPowder,
    RevivalHerb,
    Ether,
    MaxEther,
    Elixir,
    MaxElixir,
    LavaCookie,
    BerryJuice,
    SacredAsh,
    HPUp,
    Protein,
    Iron,
    Carbos,
    Calcium,
    RareCandy,
    PPUp,
    Zinc,
    PPMax,
    OldGateau,
    GuardSpec,
    DireHit,
    XAttack,
    XDefend,
    XSpeed,
    XAccuracy,
    XSpecial,
    XSpDef,
    PokeDoll,
    FluffyTail,
    BlueFlute,
    YellowFlute,
    RedFlute,
    BlackFlute,
    WhiteFlute,
    ShoalSalt,
    ShoalShell,
    RedShard,
    BlueShard,
    YellowShard,
    GreenShard,
    SuperRepel,
    MaxRepel,
    EscapeRope,
    Repel,
    SunStone,
    MoonStone,
    FireStone,
    Thunderstone,
    WaterStone,
    LeafStone,
    TinyMushroom,
    BigMushroom,
    Pearl,
    BigPearl,
    Stardust,
    StarPiece,
    Nugget,
    HeartScale,
    Honey,
    GrowthMulch,
    DampMulch,
    StableMulch,
    GooeyMulch,
    RootFossil,
    ClawFossil,
    HelixFossil,
    DomeFossil,
    OldAmber,
    ArmorFossil,
    SkullFossil,
    RareBone,
    ShinyStone,
    DuskStone,
    DawnStone,
    OvalStone,
    OddKeystone,
    GriseousOrb,
    unknown1,
    unknown2,
    unknown3,
    unknown4,
    unknown5,
    unknown6,
    unknown7,
    unknown8,
    unknown9,
    unknown10,
    unknown11,
    unknown12,
    unknown13,
    unknown14,
    unknown15,
    unknown16,
    unknown17,
    unknown18,
    unknown19,
    unknown20,
    unknown21,
    unknown22,
    AdamantOrb,
    LustrousOrb,
    GrassMail,
    FlameMail,
    BubbleMail,
    BloomMail,
    TunnelMail,
    SteelMail,
    HeartMail,
    SnowMail,
    SpaceMail,
    AirMail,
    MosaicMail,
    BrickMail,
    CheriBerry,
    ChestoBerry,
    PechaBerry,
    RawstBerry,
    AspearBerry,
    LeppaBerry,
    OranBerry,
    PersimBerry,
    LumBerry,
    SitrusBerry,
    FigyBerry,
    WikiBerry,
    MagoBerry,
    AguavBerry,
    IapapaBerry,
    RazzBerry,
    BlukBerry,
    NanabBerry,
    WepearBerry,
    PinapBerry,
    PomegBerry,
    KelpsyBerry,
    QualotBerry,
    HondewBerry,
    GrepaBerry,
    TamatoBerry,
    CornnBerry,
    MagostBerry,
    RabutaBerry,
    NomelBerry,
    SpelonBerry,
    PamtreBerry,
    WatmelBerry,
    DurinBerry,
    BelueBerry,
    OccaBerry,
    PasshoBerry,
    WacanBerry,
    RindoBerry,
    YacheBerry,
    ChopleBerry,
    KebiaBerry,
    ShucaBerry,
    CobaBerry,
    PayapaBerry,
    TangaBerry,
    ChartiBerry,
    KasibBerry,
    HabanBerry,
    ColburBerry,
    BabiriBerry,
    ChilanBerry,
    LiechiBerry,
    GanlonBerry,
    SalacBerry,
    PetayaBerry,
    ApicotBerry,
    LansatBerry,
    StarfBerry,
    EnigmaBerry,
    MicleBerry,
    CustapBerry,
    JabocaBerry,
    RowapBerry,
    BrightPowder,
    WhiteHerb,
    MachoBrace,
    ExpShare,
    QuickClaw,
    SootheBell,
    MentalHerb,
    ChoiceBand,
    KingsRock,
    SilverPowder,
    AmuletCoin,
    CleanseTag,
    SoulDew,
    DeepSeaTooth,
    DeepSeaScale,
    SmokeBall,
    Everstone,
    FocusBand,
    LuckyEgg,
    ScopeLens,
    MetalCoat,
    Leftovers,
    DragonScale,
    LightBall,
    SoftSand,
    HardStone,
    MiracleSeed,
    BlackGlasses,
    BlackBelt,
    Magnet,
    MysticWater,
    SharpBeak,
    PoisonBarb,
    NeverMeltIce,
    SpellTag,
    TwistedSpoon,
    Charcoal,
    DragonFang,
    SilkScarf,
    UpGrade,
    ShellBell,
    SeaIncense,
    LaxIncense,
    LuckyPunch,
    MetalPowder,
    ThickClub,
    Stick,
    RedScarf,
    BlueScarf,
    PinkScarf,
    GreenScarf,
    YellowScarf,
    WideLens,
    MuscleBand,
    WiseGlasses,
    ExpertBelt,
    LightClay,
    LifeOrb,
    PowerHerb,
    ToxicOrb,
    FlameOrb,
    QuickPowder,
    FocusSash,
    ZoomLens,
    Metronome,
    IronBall,
    LaggingTail,
    DestinyKnot,
    BlackSludge,
    IcyRock,
    SmoothRock,
    HeatRock,
    DampRock,
    GripClaw,
    ChoiceScarf,
    StickyBarb,
    PowerBracer,
    PowerBelt,
    PowerLens,
    PowerBand,
    PowerAnklet,
    PowerWeight,
    ShedShell,
    BigRoot,
    ChoiceSpecs,
    FlamePlate,
    SplashPlate,
    ZapPlate,
    MeadowPlate,
    IciclePlate,
    FistPlate,
    ToxicPlate,
    EarthPlate,
    SkyPlate,
    MindPlate,
    InsectPlate,
    StonePlate,
    SpookyPlate,
    DracoPlate,
    DreadPlate,
    IronPlate,
    OddIncense,
    RockIncense,
    FullIncense,
    WaveIncense,
    RoseIncense,
    LuckIncense,
    PureIncense,
    Protector,
    Electirizer,
    Magmarizer,
    DubiousDisc,
    ReaperCloth,
    RazorClaw,
    RazorFang,
    TM01,
    TM02,
    TM03,
    TM04,
    TM05,
    TM06,
    TM07,
    TM08,
    TM09,
    TM10,
    TM11,
    TM12,
    TM13,
    TM14,
    TM15,
    TM16,
    TM17,
    TM18,
    TM19,
    TM20,
    TM21,
    TM22,
    TM23,
    TM24,
    TM25,
    TM26,
    TM27,
    TM28,
    TM29,
    TM30,
    TM31,
    TM32,
    TM33,
    TM34,
    TM35,
    TM36,
    TM37,
    TM38,
    TM39,
    TM40,
    TM41,
    TM42,
    TM43,
    TM44,
    TM45,
    TM46,
    TM47,
    TM48,
    TM49,
    TM50,
    TM51,
    TM52,
    TM53,
    TM54,
    TM55,
    TM56,
    TM57,
    TM58,
    TM59,
    TM60,
    TM61,
    TM62,
    TM63,
    TM64,
    TM65,
    TM66,
    TM67,
    TM68,
    TM69,
    TM70,
    TM71,
    TM72,
    TM73,
    TM74,
    TM75,
    TM76,
    TM77,
    TM78,
    TM79,
    TM80,
    TM81,
    TM82,
    TM83,
    TM84,
    TM85,
    TM86,
    TM87,
    TM88,
    TM89,
    TM90,
    TM91,
    TM92,
    HM01,
    HM02,
    HM03,
    HM04,
    HM05,
    HM06,
    HM07,
    HM08,
    ExplorerKit,
    LootSack,
    RuleBook,
    PokeRadar,
    PointCard,
    Journal,
    SealCase,
    FashionCase,
    SealBag,
    PalPad,
    WorksKey,
    OldCharm,
    GalacticKey,
    RedChain,
    TownMap,
    VsSeeker,
    CoinCase,
    OldRod,
    GoodRod,
    SuperRod,
    Sprayduck,
    PoffinCase,
    Bicycle,
    SuiteKey,
    OaksLetter,
    LunarWing,
    MemberCard,
    AzureFlute,
    SSTicket,
    ContestPass,
    MagmaStone,
    Parcel,
    Coupon1,
    Coupon2,
    Coupon3,
    StorageKey,
    SecretPotion,
    VsRecorder,
    GracideaFlower,
    SecretKey,
    NumberOfItems,
};

inline std::string toString(Items item) {
    static const std::unordered_map<Items, std::string> itemsToString = {
        {Items::None, "none"},
        {Items::MasterBall, "masterball"},
        {Items::UltraBall, "ultraball"},
        {Items::GreatBall, "greatball"},
        {Items::PokeBall, "pokeball"},
        {Items::SafariBall, "safariball"},
        {Items::NetBall, "netball"},
        {Items::DiveBall, "diveball"},
        {Items::NestBall, "nestball"},
        {Items::RepeatBall, "repeatball"},
        {Items::TimerBall, "timerball"},
        {Items::LuxuryBall, "luxuryball"},
        {Items::PremierBall, "premierball"},
        {Items::DuskBall, "duskball"},
        {Items::HealBall, "healball"},
        {Items::QuickBall, "quickball"},
        {Items::CherishBall, "cherishball"},
        {Items::Potion, "potion"},
        {Items::Antidote, "antidote"},
        {Items::BurnHeal, "burnheal"},
        {Items::IceHeal, "iceheal"},
        {Items::Awakening, "awakening"},
        {Items::ParlyzHeal, "parlyzheal"},
        {Items::FullRestore, "fullrestore"},
        {Items::MaxPotion, "maxpotion"},
        {Items::HyperPotion, "hyperpotion"},
        {Items::SuperPotion, "superpotion"},
        {Items::FullHeal, "fullheal"},
        {Items::Revive, "revive"},
        {Items::MaxRevive, "maxrevive"},
        {Items::FreshWater, "freshwater"},
        {Items::SodaPop, "sodapop"},
        {Items::Lemonade, "lemonade"},
        {Items::MoomooMilk, "moomoomilk"},
        {Items::EnergyPowder, "energypowder"},
        {Items::EnergyRoot, "energyroot"},
        {Items::HealPowder, "healpowder"},
        {Items::RevivalHerb, "revivalherb"},
        {Items::Ether, "ether"},
        {Items::MaxEther, "maxether"},
        {Items::Elixir, "elixir"},
        {Items::MaxElixir, "maxelixir"},
        {Items::LavaCookie, "lavacookie"},
        {Items::BerryJuice, "berryjuice"},
        {Items::SacredAsh, "sacredash"},
        {Items::HPUp, "hpup"},
        {Items::Protein, "protein"},
        {Items::Iron, "iron"},
        {Items::Carbos, "carbos"},
        {Items::Calcium, "calcium"},
        {Items::RareCandy, "rarecandy"},
        {Items::PPUp, "ppup"},
        {Items::Zinc, "zinc"},
        {Items::PPMax, "ppmax"},
        {Items::OldGateau, "oldgateau"},
        {Items::GuardSpec, "guardspec"},
        {Items::DireHit, "direhit"},
        {Items::XAttack, "xattack"},
        {Items::XDefend, "xdefend"},
        {Items::XSpeed, "xspeed"},
        {Items::XAccuracy, "xaccuracy"},
        {Items::XSpecial, "xspecial"},
        {Items::XSpDef, "xspdef"},
        {Items::PokeDoll, "pokedoll"},
        {Items::FluffyTail, "fluffytail"},
        {Items::BlueFlute, "blueflute"},
        {Items::YellowFlute, "yellowflute"},
        {Items::RedFlute, "redflute"},
        {Items::BlackFlute, "blackflute"},
        {Items::WhiteFlute, "whiteflute"},
        {Items::ShoalSalt, "shoalsalt"},
        {Items::ShoalShell, "shoalshell"},
        {Items::RedShard, "redshard"},
        {Items::BlueShard, "blueshard"},
        {Items::YellowShard, "yellowshard"},
        {Items::GreenShard, "greenshard"},
        {Items::SuperRepel, "superrepel"},
        {Items::MaxRepel, "maxrepel"},
        {Items::EscapeRope, "escaperope"},
        {Items::Repel, "repel"},
        {Items::SunStone, "sunstone"},
        {Items::MoonStone, "moonstone"},
        {Items::FireStone, "firestone"},
        {Items::Thunderstone, "thunderstone"},
        {Items::WaterStone, "waterstone"},
        {Items::LeafStone, "leafstone"},
        {Items::TinyMushroom, "tinymushroom"},
        {Items::BigMushroom, "bigmushroom"},
        {Items::Pearl, "pearl"},
        {Items::BigPearl, "bigpearl"},
        {Items::Stardust, "stardust"},
        {Items::StarPiece, "starpiece"},
        {Items::Nugget, "nugget"},
        {Items::HeartScale, "heartscale"},
        {Items::Honey, "honey"},
        {Items::GrowthMulch, "growthmulch"},
        {Items::DampMulch, "dampmulch"},
        {Items::StableMulch, "stablemulch"},
        {Items::GooeyMulch, "gooeymulch"},
        {Items::RootFossil, "rootfossil"},
        {Items::ClawFossil, "clawfossil"},
        {Items::HelixFossil, "helixfossil"},
        {Items::DomeFossil, "domefossil"},
        {Items::OldAmber, "oldamber"},
        {Items::ArmorFossil, "armorfossil"},
        {Items::SkullFossil, "skullfossil"},
        {Items::RareBone, "rarebone"},
        {Items::ShinyStone, "shinystone"},
        {Items::DuskStone, "duskstone"},
        {Items::DawnStone, "dawnstone"},
        {Items::OvalStone, "ovalstone"},
        {Items::OddKeystone, "oddkeystone"},
        {Items::GriseousOrb, "griseousorb"},
        {Items::unknown1, "unknown1"},
        {Items::unknown2, "unknown2"},
        {Items::unknown3, "unknown3"},
        {Items::unknown4, "unknown4"},
        {Items::unknown5, "unknown5"},
        {Items::unknown6, "unknown6"},
        {Items::unknown7, "unknown7"},
        {Items::unknown8, "unknown8"},
        {Items::unknown9, "unknown9"},
        {Items::unknown10, "unknown10"},
        {Items::unknown11, "unknown11"},
        {Items::unknown12, "unknown12"},
        {Items::unknown13, "unknown13"},
        {Items::unknown14, "unknown14"},
        {Items::unknown15, "unknown15"},
        {Items::unknown16, "unknown16"},
        {Items::unknown17, "unknown17"},
        {Items::unknown18, "unknown18"},
        {Items::unknown19, "unknown19"},
        {Items::unknown20, "unknown20"},
        {Items::unknown21, "unknown21"},
        {Items::unknown22, "unknown22"},
        {Items::AdamantOrb, "adamantorb"},
        {Items::LustrousOrb, "lustrousoorb"},
        {Items::GrassMail, "grassmail"},
        {Items::FlameMail, "flamemail"},
        {Items::BubbleMail, "bubblemail"},
        {Items::BloomMail, "bloommail"},
        {Items::TunnelMail, "tunnelmail"},
        {Items::SteelMail, "steelmail"},
        {Items::HeartMail, "heartmail"},
        {Items::SnowMail, "snowmail"},
        {Items::SpaceMail, "spacemail"},
        {Items::AirMail, "airmail"},
        {Items::MosaicMail, "mosaicmail"},
        {Items::BrickMail, "brickmail"},
        {Items::CheriBerry, "cheriberry"},
        {Items::ChestoBerry, "chestoberry"},
        {Items::PechaBerry, "pechaberry"},
        {Items::RawstBerry, "rawstberry"},
        {Items::AspearBerry, "aspearberry"},
        {Items::LeppaBerry, "leppaberry"},
        {Items::OranBerry, "oranberry"},
        {Items::PersimBerry, "persimberry"},
        {Items::LumBerry, "lumberry"},
        {Items::SitrusBerry, "sitrusberry"},
        {Items::FigyBerry, "figyberry"},
        {Items::WikiBerry, "wikiberry"},
        {Items::MagoBerry, "magoberry"},
        {Items::AguavBerry, "aguavberry"},
        {Items::IapapaBerry, "iapapaberry"},
        {Items::RazzBerry, "razzberry"},
        {Items::BlukBerry, "blukberry"},
        {Items::NanabBerry, "nanabberry"},
        {Items::WepearBerry, "wepearberry"},
        {Items::PinapBerry, "pinapberry"},
        {Items::PomegBerry, "pomegberry"},
        {Items::KelpsyBerry, "kelpsyberry"},
        {Items::QualotBerry, "qualotberry"},
        {Items::HondewBerry, "hondewberry"},
        {Items::GrepaBerry, "grepaberry"},
        {Items::TamatoBerry, "tamatoberry"},
        {Items::CornnBerry, "cornnberry"},
        {Items::MagostBerry, "magostberry"},
        {Items::RabutaBerry, "rabutaberry"},
        {Items::NomelBerry, "nomelberry"},
        {Items::SpelonBerry, "spelonberry"},
        {Items::PamtreBerry, "pamtreberry"},
        {Items::WatmelBerry, "watmelberry"},
        {Items::DurinBerry, "durinberry"},
        {Items::BelueBerry, "belueberry"},
        {Items::OccaBerry, "occaberry"},
        {Items::PasshoBerry, "passhoberry"},
        {Items::WacanBerry, "wacanberry"},
        {Items::RindoBerry, "rindoberry"},
        {Items::YacheBerry, "yacheberry"},
        {Items::ChopleBerry, "chopleberry"},
        {Items::KebiaBerry, "kebiaberry"},
        {Items::ShucaBerry, "shucaberry"},
        {Items::CobaBerry, "cobaberry"},
        {Items::PayapaBerry, "payapaberry"},
        {Items::TangaBerry, "tangaberry"},
        {Items::ChartiBerry, "chartiberry"},
        {Items::KasibBerry, "kasibberry"},
        {Items::HabanBerry, "habanberry"},
        {Items::ColburBerry, "colburberry"},
        {Items::BabiriBerry, "babiriberry"},
        {Items::ChilanBerry, "chilanberry"},
        {Items::LiechiBerry, "liechiberry"},
        {Items::GanlonBerry, "ganlonberry"},
        {Items::SalacBerry, "salacberry"},
        {Items::PetayaBerry, "petayaberry"},
        {Items::ApicotBerry, "apicotberry"},
        {Items::LansatBerry, "lansatberry"},
        {Items::StarfBerry, "starfberry"},
        {Items::EnigmaBerry, "enigmaberry"},
        {Items::MicleBerry, "micleberry"},
        {Items::CustapBerry, "custapberry"},
        {Items::JabocaBerry, "jabocaberry"},
        {Items::RowapBerry, "rowapberry"},
        {Items::BrightPowder, "brightpowder"},
        {Items::WhiteHerb, "whiteherb"},
        {Items::MachoBrace, "machobrace"},
        {Items::ExpShare, "expshare"},
        {Items::QuickClaw, "quickclaw"},
        {Items::SootheBell, "soothebell"},
        {Items::MentalHerb, "mentalherb"},
        {Items::ChoiceBand, "choiceband"},
        {Items::KingsRock, "kingsrock"},
        {Items::SilverPowder, "silverpowder"},
        {Items::AmuletCoin, "amuletcoin"},
        {Items::CleanseTag, "cleansetag"},
        {Items::SoulDew, "souldew"},
        {Items::DeepSeaTooth, "deepseatooth"},
        {Items::DeepSeaScale, "deepseascale"},
        {Items::SmokeBall, "smokeball"},
        {Items::Everstone, "everstone"},
        {Items::FocusBand, "focusband"},
        {Items::LuckyEgg, "luckyegg"},
        {Items::ScopeLens, "scopelens"},
        {Items::MetalCoat, "metalcoat"},
        {Items::Leftovers, "leftovers"},
        {Items::DragonScale, "dragonscale"},
        {Items::LightBall, "lightball"},
        {Items::SoftSand, "softsand"},
        {Items::HardStone, "hardstone"},
        {Items::MiracleSeed, "miracleseed"},
        {Items::BlackGlasses, "blackglasses"},
        {Items::BlackBelt, "blackbelt"},
        {Items::Magnet, "magnet"},
        {Items::MysticWater, "mysticwater"},
        {Items::SharpBeak, "sharpbeak"},
        {Items::PoisonBarb, "poisonbarb"},
        {Items::NeverMeltIce, "nevermeltice"},
        {Items::SpellTag, "spelltag"},
        {Items::TwistedSpoon, "twistedspoon"},
        {Items::Charcoal, "charcoal"},
        {Items::DragonFang, "dragonfang"},
        {Items::SilkScarf, "silkscarf"},
        {Items::UpGrade, "upgrade"},
        {Items::ShellBell, "shellbell"},
        {Items::SeaIncense, "seaincense"},
        {Items::LaxIncense, "laxincense"},
        {Items::LuckyPunch, "luckypunch"},
        {Items::MetalPowder, "metalpowder"},
        {Items::ThickClub, "thickclub"},
        {Items::Stick, "stick"},
        {Items::RedScarf, "redscarf"},
        {Items::BlueScarf, "bluescarf"},
        {Items::PinkScarf, "pinkscarf"},
        {Items::GreenScarf, "greenscarf"},
        {Items::YellowScarf, "yellowscarf"},
        {Items::WideLens, "widelens"},
        {Items::MuscleBand, "muscleband"},
        {Items::WiseGlasses, "wiseglasses"},
        {Items::ExpertBelt, "expertbelt"},
        {Items::LightClay, "lightclay"},
        {Items::LifeOrb, "lifeorb"},
        {Items::PowerHerb, "powerherb"},
        {Items::ToxicOrb, "toxicorb"},
        {Items::FlameOrb, "flameorb"},
        {Items::QuickPowder, "quickpowder"},
        {Items::FocusSash, "focussash"},
        {Items::ZoomLens, "zoomlens"},
        {Items::Metronome, "metronome"},
        {Items::IronBall, "ironball"},
        {Items::LaggingTail, "laggingtail"},
        {Items::DestinyKnot, "destinyknot"},
        {Items::BlackSludge, "blacksludge"},
        {Items::IcyRock, "icyrock"},
        {Items::SmoothRock, "smoothrock"},
        {Items::HeatRock, "heatrock"},
        {Items::DampRock, "damprock"},
        {Items::GripClaw, "gripclaw"},
        {Items::ChoiceScarf, "choicescarf"},
        {Items::StickyBarb, "stickybarb"},
        {Items::PowerBracer, "powerbracer"},
        {Items::PowerBelt, "powerbelt"},
        {Items::PowerLens, "powerlens"},
        {Items::PowerBand, "powerband"},
        {Items::PowerAnklet, "poweranklet"},
        {Items::PowerWeight, "powerweight"},
        {Items::ShedShell, "shedshell"},
        {Items::BigRoot, "bigroot"},
        {Items::ChoiceSpecs, "choicespecs"},
        {Items::FlamePlate, "flameplate"},
        {Items::SplashPlate, "splashplate"},
        {Items::ZapPlate, "zapplate"},
        {Items::MeadowPlate, "meadowplate"},
        {Items::IciclePlate, "icicleplate"},
        {Items::FistPlate, "fistplate"},
        {Items::ToxicPlate, "toxicplate"},
        {Items::EarthPlate, "earthplate"},
        {Items::SkyPlate, "skyplate"},
        {Items::MindPlate, "mindplate"},
        {Items::InsectPlate, "insectplate"},
        {Items::StonePlate, "stoneplate"},
        {Items::SpookyPlate, "spookyplate"},
        {Items::DracoPlate, "dracoplate"},
        {Items::DreadPlate, "dreadplate"},
        {Items::IronPlate, "ironplate"},
        {Items::OddIncense, "oddincense"},
        {Items::RockIncense, "rockincense"},
        {Items::FullIncense, "fullincense"},
        {Items::WaveIncense, "waveincense"},
        {Items::RoseIncense, "roseincense"},
        {Items::LuckIncense, "luckincense"},
        {Items::PureIncense, "pureincense"},
        {Items::Protector, "protector"},
        {Items::Electirizer, "electirizer"},
        {Items::Magmarizer, "magmarizer"},
        {Items::DubiousDisc, "dubiousdisc"},
        {Items::ReaperCloth, "reapercloth"},
        {Items::RazorClaw, "razorclaw"},
        {Items::RazorFang, "razorfang"},
        {Items::TM01, "tm01"},
        {Items::TM02, "tm02"},
        {Items::TM03, "tm03"},
        {Items::TM04, "tm04"},
        {Items::TM05, "tm05"},
        {Items::TM06, "tm06"},
        {Items::TM07, "tm07"},
        {Items::TM08, "tm08"},
        {Items::TM09, "tm09"},
        {Items::TM10, "tm10"},
        {Items::TM11, "tm11"},
        {Items::TM12, "tm12"},
        {Items::TM13, "tm13"},
        {Items::TM14, "tm14"},
        {Items::TM15, "tm15"},
        {Items::TM16, "tm16"},
        {Items::TM17, "tm17"},
        {Items::TM18, "tm18"},
        {Items::TM19, "tm19"},
        {Items::TM20, "tm20"},
        {Items::TM21, "tm21"},
        {Items::TM22, "tm22"},
        {Items::TM23, "tm23"},
        {Items::TM24, "tm24"},
        {Items::TM25, "tm25"},
        {Items::TM26, "tm26"},
        {Items::TM27, "tm27"},
        {Items::TM28, "tm28"},
        {Items::TM29, "tm29"},
        {Items::TM30, "tm30"},
        {Items::TM31, "tm31"},
        {Items::TM32, "tm32"},
        {Items::TM33, "tm33"},
        {Items::TM34, "tm34"},
        {Items::TM35, "tm35"},
        {Items::TM36, "tm36"},
        {Items::TM37, "tm37"},
        {Items::TM38, "tm38"},
        {Items::TM39, "tm39"},
        {Items::TM40, "tm40"},
        {Items::TM41, "tm41"},
        {Items::TM42, "tm42"},
        {Items::TM43, "tm43"},
        {Items::TM44, "tm44"},
        {Items::TM45, "tm45"},
        {Items::TM46, "tm46"},
        {Items::TM47, "tm47"},
        {Items::TM48, "tm48"},
        {Items::TM49, "tm49"},
        {Items::TM50, "tm50"},
        {Items::TM51, "tm51"},
        {Items::TM52, "tm52"},
        {Items::TM53, "tm53"},
        {Items::TM54, "tm54"},
        {Items::TM55, "tm55"},
        {Items::TM56, "tm56"},
        {Items::TM57, "tm57"},
        {Items::TM58, "tm58"},
        {Items::TM59, "tm59"},
        {Items::TM60, "tm60"},
        {Items::TM61, "tm61"},
        {Items::TM62, "tm62"},
        {Items::TM63, "tm63"},
        {Items::TM64, "tm64"},
        {Items::TM65, "tm65"},
        {Items::TM66, "tm66"},
        {Items::TM67, "tm67"},
        {Items::TM68, "tm68"},
        {Items::TM69, "tm69"},
        {Items::TM70, "tm70"},
        {Items::TM71, "tm71"},
        {Items::TM72, "tm72"},
        {Items::TM73, "tm73"},
        {Items::TM74, "tm74"},
        {Items::TM75, "tm75"},
        {Items::TM76, "tm76"},
        {Items::TM77, "tm77"},
        {Items::TM78, "tm78"},
        {Items::TM79, "tm79"},
        {Items::TM80, "tm80"},
        {Items::TM81, "tm81"},
        {Items::TM82, "tm82"},
        {Items::TM83, "tm83"},
        {Items::TM84, "tm84"},
        {Items::TM85, "tm85"},
        {Items::TM86, "tm86"},
        {Items::TM87, "tm87"},
        {Items::TM88, "tm88"},
        {Items::TM89, "tm89"},
        {Items::TM90, "tm90"},
        {Items::TM91, "tm91"},
        {Items::TM92, "tm92"},
        {Items::HM01, "hm01"},
        {Items::HM02, "hm02"},
        {Items::HM03, "hm03"},
        {Items::HM04, "hm04"},
        {Items::HM05, "hm05"},
        {Items::HM06, "hm06"},
        {Items::HM07, "hm07"},
        {Items::HM08, "hm08"},
        {Items::ExplorerKit, "explorerkit"},
        {Items::LootSack, "lootsack"},
        {Items::RuleBook, "rulebook"},
        {Items::PokeRadar, "pokeradar"},
        {Items::PointCard, "pointcard"},
        {Items::Journal, "journal"},
        {Items::SealCase, "sealcase"},
        {Items::FashionCase, "fashioncase"},
        {Items::SealBag, "sealbag"},
        {Items::PalPad, "palpad"},
        {Items::WorksKey, "workskey"},
        {Items::OldCharm, "oldcharm"},
        {Items::GalacticKey, "galactickey"},
        {Items::RedChain, "redchain"},
        {Items::TownMap, "townmap"},
        {Items::VsSeeker, "vsseeker"},
        {Items::CoinCase, "coincase"},
        {Items::OldRod, "oldrod"},
        {Items::GoodRod, "goodrod"},
        {Items::SuperRod, "superrod"},
        {Items::Sprayduck, "sprayduck"},
        {Items::PoffinCase, "poffincase"},
        {Items::Bicycle, "bicycle"},
        {Items::SuiteKey, "suitekey"},
        {Items::OaksLetter, "oaksletter"},
        {Items::LunarWing, "lunarwing"},
        {Items::MemberCard, "membercard"},
        {Items::AzureFlute, "azureflute"},
        {Items::SSTicket, "ssticket"},
        {Items::ContestPass, "contestpass"},
        {Items::MagmaStone, "magmastone"},
        {Items::Parcel, "parcel"},
        {Items::Coupon1, "coupon1"},
        {Items::Coupon2, "coupon2"},
        {Items::Coupon3, "coupon3"},
        {Items::StorageKey, "storagekey"},
        {Items::SecretPotion, "secretpotion"},
        {Items::VsRecorder, "vsrecorder"},
        {Items::GracideaFlower, "gracideaflower"},
        {Items::SecretKey, "secretkey"}
    };

    auto it = itemsToString.find(item);
    if (it != itemsToString.end()) {
        return it->second;
    } else {
        return "unknown";
    }
}