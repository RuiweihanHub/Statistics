#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <limits> // 用于输入验证
#include <sstream> // 用于字符串流处理
#include <conio.h> // 用于_kbhit()和_getch()函数
using namespace std;

/* ==========================  数据定义  ========================== */

// 背包物品结构
struct InventoryItem {
    string name;
    int count;
};

struct UserData {
    string username;
    string password;
    int money = 0;
    int experience = 0;
    int lotteryCount = 0;
    int totalMining = 0;
    int totalLottery = 0;
    string registerDate;
    string lastLoginDate;

    /* ---- 生存属性 ---- */
    int health = 100;
    int maxHealth = 100;
    int fatigue = 100;
    int maxFatigue = 100;
    int hunger = 100;
    int thirst = 100;
    int warmth = 100;      // 新增：温暖值
    int fear = 0;          // 新增：恐惧值 (0-100)
    int oxygen = 100;

    /* ---- 消耗品 ---- */
    int food = 5;
    int water = 5;
    int bandage = 3;
    int trapWire = 5;
    int mine = 2;
    int bed = 0;
    int bedDurability = 0;

    /* ---- 镐子 ---- */
    int woodPickaxe = 1;
    int stonePickaxe = 0;
    int copperPickaxe = 0;
    int ironPickaxe = 0;
    int diamondPickaxe = 0;
    int magicPickaxe = 0;

    int woodPickaxeDurability = 50;
    int stonePickaxeDurability = 0;
    int copperPickaxeDurability = 0;
    int ironPickaxeDurability = 0;
    int diamondPickaxeDurability = 0;
    int magicPickaxeDurability = 0;

    /* ---- 世界状态 ---- */
    bool isNight = false;
    int dayCount = 1;

    /* ---- 药水 ---- */
    int breathPotion = 0;
    int luckPotion = 0;
    int healthPotion = 0;
    int fatiguePotion = 0;
    int nightVisionPotion = 0;
    int strengthPotion = 0;

    /* ---- 矿图 ---- */
    bool mapLeft = false;
    bool mapRight = false;
    bool mapDown = false;

    /* ---- 游戏设置 ---- */
    bool keepInventory = false;
    bool autoLogin = false;

    /* ---- 坐标 ---- */
    int depth = 0;
    int positionX = 0;
    int positionY = 0;

    /* ---- 梯子系统 ---- */
    int ladders = 0;              // 梯子数量
    int currentLadderLength = 0;  // 当前使用的梯子长度
    int currentLadderDurability = 0; // 当前梯子耐久
    bool ladderPlaced = false;    // 是否放置了梯子

    /* ---- 背包系统 ---- */
    int backpackSize = 16;        // 背包容量
    int storageBox = 0;           // 地面箱子数量

    /* ---- 金属探测仪 ---- */
    int metalDetector = 0;        // 金属探测仪
    int batteries = 0;            // 电池
    
    /* ---- 矿石和材料 ---- */
	    int diamonds = 0;
	    int sapphires = 0;
	    int emeralds = 0;
	    int gold = 0;
	    int silver = 0;
	    int copper = 0;
	    int granite = 0;
	    int iron = 0;
	    int wood = 0;
	    int coal = 0;
};

UserData currentUser;
// 配置结构体
typedef struct {
    bool quickStart;
    bool hasBuiltinAdmin;
    string adminPassword;
} GameConfig;

GameConfig gameConfig = {false, true, "12345678"};

// 函数声明
void loadGameConfig();
void saveGameConfig();

/* ==========================  价格表  ========================== */
map<string, int> itemPrices = {
    {"钻石", 10000}, {"蓝宝石", 5000}, {"绿宝石", 3000}, {"黄金", 2000},
    {"白银", 800}, {"铜", 300}, {"花岗岩", 100}, {"铁", 50},
    {"木头", 20}, {"煤炭", 30}, {"食物", 50}, {"水", 30}, {"绷带", 80},
    {"憋气药水", 200}, {"运气药水", 300},
    {"生命药水", 400}, {"抗疲劳药水", 250}, {"夜视药水", 350}, {"力量药水", 450},
    {"左矿图", 1000}, {"右矿图", 1000}, {"下矿图", 1500},
    {"木床", 500}, {"木镐", 100}, {"石镐", 300}, {"铜镐", 800},
    {"铁镐", 1500}, {"钻石镐", 5000}, {"魔石镐", 10000},
    {"木梯(10米)", 200}, {"木梯(20米)", 350}, {"铁梯(50米)", 1000}, {"钻石梯(无限)", 50000},
    // 新增物品
    {"背包扩容", 2000}, {"储物箱", 1500}, {"金属探测仪", 3000}, {"电池", 100}
};

/* ==========================  矿脉数据  ========================== */
struct OreDeposit {
    string type;
    int amount;
    int x, y, depth;
};
vector<OreDeposit> oreDeposits;

/* ==========================  僵尸数据  ========================== */
struct Zombie {
    int health;
    int damage;
    string type;
    int x, y, depth;
};
vector<Zombie> zombies;

/* ==========================  成就系统  ========================== */
struct Achievement {
    string name;
    string description;
    bool unlocked;
    int requirement;
    string rewardType;
    int rewardValue;
};
vector<Achievement> achievements = {
    {"初出茅庐", "第一次挖矿", false, 1, "experience", 200},
    {"抽奖新手", "第一次抽奖", false, 1, "experience", 100},
    {"矿工学徒", "挖矿10次", false, 10, "experience", 300},
    {"幸运儿", "抽奖5次", false, 5, "lottery", 2},
    {"资源大亨", "拥有10000元", false, 10000, "experience", 1000},
    {"资深矿工", "挖矿50次", false, 50, "experience", 1500},
    {"宝石收藏家", "拥有5颗钻石", false, 5, "experience", 800},
    {"百万富翁", "拥有100000元", false, 100000, "experience", 5000},
    {"僵尸杀手", "杀死10只僵尸", false, 10, "experience", 1200},
    {"生存专家", "在深矿中生存", false, 1, "health", 50},
    {"攀登者", "放置第一条梯子", false, 1, "experience", 300},
    {"高空作业", "使用梯子到达50米深度", false, 50, "diamonds", 2},
    // 新增成就
    {"探险家", "背包扩容到24格", false, 24, "backpack", 8},
    {"寻宝者", "使用金属探测仪20次", false, 20, "experience", 1000},
    {"冷静之人", "恐惧值低于20挖矿100次", false, 100, "healthPotion", 3}
};

string Statistics = R"(
###################################################################
#       ____ _____  _  _____ ___ ____ _____ ___ ____ ____         #
#      / ___|_   _|/ \|_   _|_ _/ ___|_   _|_ _/ ___/ ___|        #
#      \___ \ | | / _ \ | |  | |\___ \ | |  | | |   \___ \        #
#       ___) || |/ ___ \| |  | | ___) || |  | | |___ ___) |       #
#      |____/ |_/_/   \_\_| |___|____/ |_| |___|\____|____/       #
#                                                                 #
###################################################################
)";

// 函数声明
string getCurrentDate();
void showProgressBar(int duration = 70);
bool checkSurvival();
void showStatusBar();
void generateOreDeposits();
void generateZombies();
void checkAchievements();
void saveUserData();
bool loadUserData(const string& username);
void checkDailyReset();
int getTotalItemCount();
bool hasBackpackSpace(int count = 1);
void showInventory();
void useItems();
void buyItems();
void sellItems();
string checkNearbyOres(int direction);
void encounterZombie();
void mining();
void lottery();
void showUserInfo();
void showAchievements();
void showLeaderboard();
void debugConsole();
void shop();
void settings();
bool login();
bool registerUser();
void updateTime();
int getCurrentPickaxe();
int getCurrentPickaxeDurability();
void usePickaxeDurability();
void showPickaxeStatus();
void showLadderStatus();
bool canReturnToSurface();
void useMetalDetector();
void centerText(const string& text);

/* ==========================  工具函数  ========================== */
// 获取控制台宽度的函数
int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 100; // 默认值，以防获取失败
}

// 居中显示文本函数
void centerText(const string& text) {
    int CONSOLE_WIDTH = getConsoleWidth(); // 获取控制台宽度
    stringstream ss(text);
    string line;
    
    while (getline(ss, line)) {
        int padding = (CONSOLE_WIDTH - line.length()) / 2;
        if (padding > 0) {
            cout << string(padding, ' ') << line << endl;
        } else {
            cout << line << endl;
        }
    }
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return string(buffer);
}

void showProgressBar(int duration) {
    int totalSteps = 50;
    for (int currentStep = 0; currentStep <= totalSteps; ++currentStep) {
        float percentage = static_cast<float>(currentStep) / totalSteps * 100.0;
        int filledLength = static_cast<int>(percentage / 100.0 * 50);
        
        string progressBar = "                                [";
        progressBar += string(filledLength, '=');
        progressBar += string(50 - filledLength, ' ');
        progressBar += "] ";
        progressBar += to_string(static_cast<int>(percentage));
        progressBar += "%";
        
        cout << "\r" << progressBar << flush;
        Sleep(duration);
    }
    cout << "\n\n加载完成！" << endl;
    Sleep(1000);
}

// 获取当前使用的镐子类型
int getCurrentPickaxe() {
    if (currentUser.magicPickaxe > 0 && currentUser.magicPickaxeDurability > 0) return 6;
    if (currentUser.diamondPickaxe > 0 && currentUser.diamondPickaxeDurability > 0) return 5;
    if (currentUser.ironPickaxe > 0 && currentUser.ironPickaxeDurability > 0) return 4;
    if (currentUser.copperPickaxe > 0 && currentUser.copperPickaxeDurability > 0) return 3;
    if (currentUser.stonePickaxe > 0 && currentUser.stonePickaxeDurability > 0) return 2;
    return 1; // 木镐
}

// 获取当前镐子耐久度
int getCurrentPickaxeDurability() {
    switch(getCurrentPickaxe()) {
        case 6: return currentUser.magicPickaxeDurability;
        case 5: return currentUser.diamondPickaxeDurability;
        case 4: return currentUser.ironPickaxeDurability;
        case 3: return currentUser.copperPickaxeDurability;
        case 2: return currentUser.stonePickaxeDurability;
        default: return currentUser.woodPickaxeDurability;
    }
}

// 使用镐子耐久度
void usePickaxeDurability() {
    int usage = 2; // 增加消耗
    if (currentUser.isNight) usage = 3; // 夜晚消耗更高
    
    switch(getCurrentPickaxe()) {
        case 6: currentUser.magicPickaxeDurability -= usage; break;
        case 5: currentUser.diamondPickaxeDurability -= usage; break;
        case 4: currentUser.ironPickaxeDurability -= usage; break;
        case 3: currentUser.copperPickaxeDurability -= usage; break;
        case 2: currentUser.stonePickaxeDurability -= usage; break;
        default: currentUser.woodPickaxeDurability -= usage; break;
    }
}

// 显示镐子状态（修复耐久条显示问题）
void showPickaxeStatus() {
    int pickaxeType = getCurrentPickaxe();
    string pickaxeName;
    int durability = getCurrentPickaxeDurability();
    
    switch(pickaxeType) {
        case 1: pickaxeName = "木镐"; break;
        case 2: pickaxeName = "石镐"; break;
        case 3: pickaxeName = "铜镐"; break;
        case 4: pickaxeName = "铁镐"; break;
        case 5: pickaxeName = "钻石镐"; break;
        case 6: pickaxeName = "魔石镐"; break;
    }
    
    // 计算百分比，统一显示为10格
    int maxDurability;
    switch(pickaxeType) {
        case 6: maxDurability = 1000; break;
        case 5: maxDurability = 500; break;
        case 4: maxDurability = 300; break;
        case 3: maxDurability = 200; break;
        case 2: maxDurability = 100; break;
        default: maxDurability = 50; break;
    }
    
    int percentage = (durability * 100) / maxDurability;
    int bars = percentage / 10;
    
    cout << "当前镐子: " << pickaxeName << " [" << string(bars, '|') 
         << string(10 - bars, ' ') << "] " << percentage << "%";
    if (percentage < 20) cout << " (即将损坏)";
    cout << endl;
}

// 显示梯子状态
void showLadderStatus() {
    if (currentUser.depth > 0 && currentUser.ladderPlaced) {
        int percentage = currentUser.currentLadderDurability;
        int bars = percentage / 10;
        cout << "梯子: [" << string(bars, '#') << string(10 - bars, ' ') << "] "
             << percentage << "% | 长度:" << currentUser.currentLadderLength << "米";
        if (currentUser.currentLadderLength < currentUser.depth) 
            cout << " (不够!)";
        cout << endl;
    } else if (currentUser.depth > 0) {
        cout << "警告: 未放置梯子，无法返回地面!" << endl;
    }
}

// 检查是否可以返回地面
bool canReturnToSurface() {
    if (currentUser.depth == 0) return true;
    if (!currentUser.ladderPlaced) return false;
    if (currentUser.currentLadderDurability <= 0) return false;
    if (currentUser.currentLadderLength < currentUser.depth) return false;
    return true;
}

// 计算总物品数量
int getTotalItemCount() {
    return currentUser.diamonds + currentUser.sapphires + currentUser.emeralds +
           currentUser.gold + currentUser.silver + currentUser.copper +
           currentUser.granite + currentUser.iron + currentUser.wood +
           currentUser.coal + currentUser.food + currentUser.water +
           currentUser.bandage + currentUser.trapWire + currentUser.mine +
           currentUser.breathPotion + currentUser.luckPotion +
           currentUser.healthPotion + currentUser.fatiguePotion +
           currentUser.nightVisionPotion + currentUser.strengthPotion;
}

// 检查背包空间
bool hasBackpackSpace(int count) {
    int currentItems = getTotalItemCount();
    return (currentItems + count) <= currentUser.backpackSize;
}

/* ==========================  存档/读档  ========================== */
void saveUserData() {
    // 对于Administrator用户，数据存储在config.ini中
    // 对于普通用户，数据存储在user_save.ini中
    string filename = (currentUser.username == "Administrator") ? "config.ini" : "user_save.ini";
    ofstream file(filename);
    if (file.is_open()) {
        // 写入用户标记，用于区分不同用户的数据
        file << "[User:" << currentUser.username << "]" << endl;
        file << "username=" << currentUser.username << endl;
        // 不保存密码（对于所有用户）
        file << "password=" << endl;
        file << "money=" << currentUser.money << endl;
        file << "experience=" << currentUser.experience << endl;
        file << "lotteryCount=" << currentUser.lotteryCount << endl;
        file << "totalMining=" << currentUser.totalMining << endl;
        file << "totalLottery=" << currentUser.totalLottery << endl;
        file << "registerDate=" << currentUser.registerDate << endl;
        file << "lastLoginDate=" << currentUser.lastLoginDate << endl;
        file << "health=" << currentUser.health << endl;
        file << "maxHealth=" << currentUser.maxHealth << endl;
        file << "fatigue=" << currentUser.fatigue << endl;
        file << "maxFatigue=" << currentUser.maxFatigue << endl;
        file << "hunger=" << currentUser.hunger << endl;
        file << "thirst=" << currentUser.thirst << endl;
        file << "warmth=" << currentUser.warmth << endl;
        file << "fear=" << currentUser.fear << endl;
        file << "oxygen=" << currentUser.oxygen << endl;
        file << "food=" << currentUser.food << endl;
        file << "water=" << currentUser.water << endl;
        file << "bandage=" << currentUser.bandage << endl;
        file << "trapWire=" << currentUser.trapWire << endl;
        file << "mine=" << currentUser.mine << endl;
        file << "bed=" << currentUser.bed << endl;
        file << "bedDurability=" << currentUser.bedDurability << endl;
        file << "woodPickaxe=" << currentUser.woodPickaxe << endl;
        file << "stonePickaxe=" << currentUser.stonePickaxe << endl;
        file << "copperPickaxe=" << currentUser.copperPickaxe << endl;
        file << "ironPickaxe=" << currentUser.ironPickaxe << endl;
        file << "diamondPickaxe=" << currentUser.diamondPickaxe << endl;
        file << "magicPickaxe=" << currentUser.magicPickaxe << endl;
        file << "woodPickaxeDurability=" << currentUser.woodPickaxeDurability << endl;
        file << "stonePickaxeDurability=" << currentUser.stonePickaxeDurability << endl;
        file << "copperPickaxeDurability=" << currentUser.copperPickaxeDurability << endl;
        file << "ironPickaxeDurability=" << currentUser.ironPickaxeDurability << endl;
        file << "diamondPickaxeDurability=" << currentUser.diamondPickaxeDurability << endl;
        file << "magicPickaxeDurability=" << currentUser.magicPickaxeDurability << endl;
        file << "isNight=" << currentUser.isNight << endl;
        file << "dayCount=" << currentUser.dayCount << endl;
        file << "breathPotion=" << currentUser.breathPotion << endl;
        file << "luckPotion=" << currentUser.luckPotion << endl;
        file << "healthPotion=" << currentUser.healthPotion << endl;
        file << "fatiguePotion=" << currentUser.fatiguePotion << endl;
        file << "nightVisionPotion=" << currentUser.nightVisionPotion << endl;
        file << "strengthPotion=" << currentUser.strengthPotion << endl;
        file << "mapLeft=" << currentUser.mapLeft << endl;
        file << "mapRight=" << currentUser.mapRight << endl;
        file << "mapDown=" << currentUser.mapDown << endl;
        file << "keepInventory=" << currentUser.keepInventory << endl;
        file << "autoLogin=" << currentUser.autoLogin << endl;
        file << "depth=" << currentUser.depth << endl;
        file << "positionX=" << currentUser.positionX << endl;
        file << "positionY=" << currentUser.positionY << endl;
        file << "ladders=" << currentUser.ladders << endl;
        file << "currentLadderLength=" << currentUser.currentLadderLength << endl;
        file << "currentLadderDurability=" << currentUser.currentLadderDurability << endl;
        file << "ladderPlaced=" << currentUser.ladderPlaced << endl;
        file << "backpackSize=" << currentUser.backpackSize << endl;
        file << "storageBox=" << currentUser.storageBox << endl;
        file << "metalDetector=" << currentUser.metalDetector << endl;
        file << "batteries=" << currentUser.batteries << endl;
        file.close();
    }
}

bool loadUserData(const string& username) {
    // 对于Administrator用户，从config.ini读取数据
    // 对于普通用户，从user_save.ini读取数据
    string filename = (username == "Administrator") ? "config.ini" : "user_save.ini";
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        bool foundUser = false;
        
        // 查找对应的用户区块
        while (getline(file, line)) {
            if (line == "[User:" + username + "]") {
                foundUser = true;
                break;
            }
        }
        
        if (!foundUser) {
            file.close();
            return false;
        }
        
        // 读取用户数据
        map<string, string> data;
        while (getline(file, line) && line[0] != '[') {
            if (line.empty()) continue;
            size_t pos = line.find('=');
            if (pos != string::npos) {
                string key = line.substr(0, pos);
                string value = line.substr(pos + 1);
                data[key] = value;
            }
        }
        
        // 填充用户数据
        currentUser.username = data["username"];
        currentUser.password = data["password"];
        currentUser.money = stoi(data["money"]);
        currentUser.experience = stoi(data["experience"]);
        currentUser.lotteryCount = stoi(data["lotteryCount"]);
        currentUser.totalMining = stoi(data["totalMining"]);
        currentUser.totalLottery = stoi(data["totalLottery"]);
        currentUser.registerDate = data["registerDate"];
        currentUser.lastLoginDate = data["lastLoginDate"];
        currentUser.health = stoi(data["health"]);
        currentUser.maxHealth = stoi(data["maxHealth"]);
        currentUser.fatigue = stoi(data["fatigue"]);
        currentUser.maxFatigue = stoi(data["maxFatigue"]);
        currentUser.hunger = stoi(data["hunger"]);
        currentUser.thirst = stoi(data["thirst"]);
        currentUser.warmth = stoi(data["warmth"]);
        currentUser.fear = stoi(data["fear"]);
        currentUser.oxygen = stoi(data["oxygen"]);
        currentUser.food = stoi(data["food"]);
        currentUser.water = stoi(data["water"]);
        currentUser.bandage = stoi(data["bandage"]);
        currentUser.trapWire = stoi(data["trapWire"]);
        currentUser.mine = stoi(data["mine"]);
        currentUser.bed = stoi(data["bed"]);
        currentUser.bedDurability = stoi(data["bedDurability"]);
        currentUser.woodPickaxe = stoi(data["woodPickaxe"]);
        currentUser.stonePickaxe = stoi(data["stonePickaxe"]);
        currentUser.copperPickaxe = stoi(data["copperPickaxe"]);
        currentUser.ironPickaxe = stoi(data["ironPickaxe"]);
        currentUser.diamondPickaxe = stoi(data["diamondPickaxe"]);
        currentUser.magicPickaxe = stoi(data["magicPickaxe"]);
        currentUser.woodPickaxeDurability = stoi(data["woodPickaxeDurability"]);
        currentUser.stonePickaxeDurability = stoi(data["stonePickaxeDurability"]);
        currentUser.copperPickaxeDurability = stoi(data["copperPickaxeDurability"]);
        currentUser.ironPickaxeDurability = stoi(data["ironPickaxeDurability"]);
        currentUser.diamondPickaxeDurability = stoi(data["diamondPickaxeDurability"]);
        currentUser.magicPickaxeDurability = stoi(data["magicPickaxeDurability"]);
        currentUser.isNight = stoi(data["isNight"]);
        currentUser.dayCount = stoi(data["dayCount"]);
        currentUser.breathPotion = stoi(data["breathPotion"]);
        currentUser.luckPotion = stoi(data["luckPotion"]);
        currentUser.healthPotion = stoi(data["healthPotion"]);
        currentUser.fatiguePotion = stoi(data["fatiguePotion"]);
        currentUser.nightVisionPotion = stoi(data["nightVisionPotion"]);
        currentUser.strengthPotion = stoi(data["strengthPotion"]);
        currentUser.mapLeft = stoi(data["mapLeft"]);
        currentUser.mapRight = stoi(data["mapRight"]);
        currentUser.mapDown = stoi(data["mapDown"]);
        currentUser.keepInventory = stoi(data["keepInventory"]);
        currentUser.autoLogin = stoi(data["autoLogin"]);
        currentUser.depth = stoi(data["depth"]);
        currentUser.positionX = stoi(data["positionX"]);
        currentUser.positionY = stoi(data["positionY"]);
        currentUser.ladders = stoi(data["ladders"]);
        currentUser.currentLadderLength = stoi(data["currentLadderLength"]);
        currentUser.currentLadderDurability = stoi(data["currentLadderDurability"]);
        currentUser.ladderPlaced = stoi(data["ladderPlaced"]);
        currentUser.backpackSize = stoi(data["backpackSize"]);
        currentUser.storageBox = stoi(data["storageBox"]);
        currentUser.metalDetector = stoi(data["metalDetector"]);
        currentUser.batteries = stoi(data["batteries"]);
        file.close();
        
        generateOreDeposits();
        return true;
    }
    return false;
}

/* ==========================  生存系统  ========================== */
bool checkSurvival() {
    // 恐惧值影响
    if (currentUser.fear >= 80) {
        cout << "恐惧值过高，你感到极度恐慌！" << endl;
    }
    
    // 温暖值影响
    if (currentUser.warmth < 20) {
        cout << "体温过低，你开始颤抖！" << endl;
    }

    if (currentUser.health <= 0) {
        cout << "\n\n==========================================" << endl;
        cout << "            你失败了！                 " << endl;
        cout << "==========================================" << endl;
        cout << "由于受伤过重，您的冒险结束了！" << endl;
        
        if (!currentUser.keepInventory) {
            int lostMoney = currentUser.money;
            currentUser.money = 0;
            cout << "你失去了所有的 " << lostMoney << " 元！" << endl;
            cout << "所有物品在失败时遗失了！" << endl;
            // 直接清空所有物品
            currentUser.diamonds = 0;
            currentUser.sapphires = 0;
            currentUser.emeralds = 0;
            currentUser.gold = 0;
            currentUser.silver = 0;
            currentUser.copper = 0;
            currentUser.granite = 0;
            currentUser.iron = 0;
            currentUser.wood = 0;
            currentUser.coal = 0;
            currentUser.food = 0;
            currentUser.water = 0;
            currentUser.bandage = 0;
            currentUser.trapWire = 0;
            currentUser.mine = 0;
            currentUser.breathPotion = 0;
            currentUser.luckPotion = 0;
            currentUser.healthPotion = 0;
            currentUser.fatiguePotion = 0;
            currentUser.nightVisionPotion = 0;
            currentUser.strengthPotion = 0;
        } else {
            cout << "开发模式：物品已保留" << endl;
        }
        
        currentUser.health = currentUser.maxHealth;
        currentUser.fatigue = currentUser.maxFatigue;
        currentUser.hunger = 100;
        currentUser.thirst = 100;
        currentUser.warmth = 100;
        currentUser.fear = 0;
        currentUser.oxygen = 100;
        currentUser.depth = 0;
        currentUser.positionX = 0;
        currentUser.positionY = 0;
        currentUser.ladderPlaced = false;
        currentUser.currentLadderLength = 0;
        currentUser.currentLadderDurability = 0;
        
        saveUserData();
        
        cout << "\n按任意键继续..." << endl;
        cin.get();
        cin.get();
        return false;
    }
    
    if (currentUser.oxygen <= 0) {
        cout << "\n\n==========================================" << endl;
        cout << "            你失败了！             " << endl;
        cout << "==========================================" << endl;
        cout << "由于缺氧，您在水中窒息了！" << endl;
        
        if (!currentUser.keepInventory) {
            int lostMoney = currentUser.money / 3;
            currentUser.money -= lostMoney;
            cout << "你失去了 " << lostMoney << " 元！" << endl;
        }
        
        currentUser.health = currentUser.maxHealth;
        currentUser.oxygen = 100;
        currentUser.fear = 0;
        currentUser.depth = 0;
        currentUser.positionX = 0;
        currentUser.positionY = 0;
        currentUser.ladderPlaced = false;
        currentUser.currentLadderLength = 0;
        currentUser.currentLadderDurability = 0;
        
        saveUserData();
        
        cout << "\n按任意键继续..." << endl;
        cin.get();
        cin.get();
        return false;
    }
    
    return true;
}

void showStatusBar() {
    cout << "生命值: [" << string(currentUser.health/10, '|') << string(10 - currentUser.health/10, ' ') << "] " 
         << currentUser.health << "/" << currentUser.maxHealth << endl;
    
    cout << "疲惫值: [" << string(currentUser.fatigue/10, '|') << string(10 - currentUser.fatigue/10, ' ') << "] " 
         << currentUser.fatigue << "/" << currentUser.maxFatigue;
    if (currentUser.fatigue < 30) cout << " (疲惫)";
    if (currentUser.fatigue < 10) cout << " (精疲力尽)";
    cout << endl;
    
    cout << "饥饿值: [" << string(currentUser.hunger/10, '|') << string(10 - currentUser.hunger/10, ' ') << "] " 
         << currentUser.hunger << "/100";
    if (currentUser.hunger < 20) cout << " (饥饿)";
    cout << endl;
    
    cout << "口渴值: [" << string(currentUser.thirst/10, '|') << string(10 - currentUser.thirst/10, ' ') << "] " 
         << currentUser.thirst << "/100";
    if (currentUser.thirst < 20) cout << " (口渴)";
    cout << endl;

    // 新增：温暖值显示
    cout << "温暖值: [" << string(currentUser.warmth/10, '|') << string(10 - currentUser.warmth/10, ' ') << "] " 
         << currentUser.warmth << "/100";
    if (currentUser.warmth < 20) cout << " (寒冷)";
    cout << endl;

    // 新增：恐惧值显示
    cout << "恐惧值: [" << string(currentUser.fear/10, '#') << string(10 - currentUser.fear/10, ' ') << "] " 
         << currentUser.fear << "/100";
    if (currentUser.fear > 70) cout << " (极度恐慌)";
    else if (currentUser.fear > 40) cout << " (紧张)";
    cout << endl;
    
    if (currentUser.oxygen < 100) {
        cout << "氧气值: [" << string(currentUser.oxygen/10, '~') << string(10 - currentUser.oxygen/10, ' ') << "] " 
             << currentUser.oxygen << "/100" << endl;
    }
    
    // 显示镐子状态
    if (currentUser.woodPickaxe > 0 || currentUser.stonePickaxe > 0 || 
        currentUser.ironPickaxe > 0 || currentUser.diamondPickaxe > 0 || 
        currentUser.magicPickaxe > 0) {
        string pickaxeName = "木镐";
        int durability = currentUser.woodPickaxeDurability;
        int maxDurability = 50;
        
        if (currentUser.magicPickaxe > 0) {
            pickaxeName = "魔石镐";
            durability = currentUser.magicPickaxeDurability;
            maxDurability = 1000;
        } else if (currentUser.diamondPickaxe > 0) {
            pickaxeName = "钻石镐";
            durability = currentUser.diamondPickaxeDurability;
            maxDurability = 500;
        } else if (currentUser.ironPickaxe > 0) {
            pickaxeName = "铁镐";
            durability = currentUser.ironPickaxeDurability;
            maxDurability = 200;
        } else if (currentUser.stonePickaxe > 0) {
            pickaxeName = "石镐";
            durability = currentUser.stonePickaxeDurability;
            maxDurability = 100;
        }
        
        // 确保耐久度不为负数
        durability = max(0, durability);
        // 计算状态条长度（最多20个字符）
        int barLength = min(20, (int)((float)durability / maxDurability * 20));
        cout << "当前镐子: " << pickaxeName << " [" << string(barLength, '|') 
             << string(20 - barLength, ' ') << "] " << durability << "/" << maxDurability << endl;
    }
    
    // 显示梯子状态
    if (currentUser.ladderPlaced) {
        cout << "梯子状态: " << currentUser.currentLadderLength << "米, 耐久: " 
             << currentUser.currentLadderDurability << endl;
    }
}

/* ==========================  时间系统  ========================== */
void updateTime() {
    // 每5次操作切换昼夜
    static int actionCount = 0;
    actionCount++;
    
    if (actionCount >= 5) {
        currentUser.isNight = !currentUser.isNight;
        actionCount = 0;
        if (!currentUser.isNight) {
            currentUser.dayCount++;
        }
    }
}

/* ==========================  世界生成  ========================== */
void generateOreDeposits() {
    oreDeposits.clear();
    
    for (int i = 0; i < 25; i++) { // 增加数量
        int amount = rand() % 5 + 1;
        if (currentUser.isNight) amount++; // 夜晚矿脉更丰富
        OreDeposit ore = {"铁", amount, rand() % 50 - 25, 0, rand() % 100 + 1};
        oreDeposits.push_back(ore);
    }
    for (int i = 0; i < 18; i++) {
        OreDeposit ore = {"铜", rand() % 4 + 2, rand() % 50 - 25, 0, rand() % 150 + 50};
        oreDeposits.push_back(ore);
    }
    for (int i = 0; i < 12; i++) {
        OreDeposit ore = {"银", rand() % 3 + 1, rand() % 50 - 25, 0, rand() % 200 + 100};
        oreDeposits.push_back(ore);
    }
    for (int i = 0; i < 10; i++) {
        OreDeposit ore = {"黄金", rand() % 2 + 1, rand() % 50 - 25, 0, rand() % 300 + 200};
        oreDeposits.push_back(ore);
    }
    for (int i = 0; i < 6; i++) {
        OreDeposit ore = {"钻石", 1, rand() % 50 - 25, 0, rand() % 500 + 300};
        oreDeposits.push_back(ore);
    }
    for (int i = 0; i < 4; i++) {
        OreDeposit ore = {"魔矿石", 1, rand() % 50 - 25, 0, rand() % 800 + 500};
        oreDeposits.push_back(ore);
    }
}

void generateZombies() {
    zombies.clear();
    
    int zombieCount = currentUser.depth / 40 + 1; // 更容易遇到
    if (currentUser.isNight) zombieCount *= 2;
    
    for (int i = 0; i < zombieCount; i++) {
        Zombie zombie;
        zombie.health = 25 + (currentUser.depth / 15);
        zombie.damage = 8 + (currentUser.depth / 25);
        zombie.type = currentUser.isNight ? "夜行僵尸" : "普通僵尸";
        if (currentUser.depth > 300) zombie.type = "深矿恶魔"; // 新增高级僵尸
        zombie.x = currentUser.positionX + rand() % 10 - 5;
        zombie.y = currentUser.positionY + rand() % 10 - 5;
        zombie.depth = currentUser.depth;
        zombies.push_back(zombie);
    }
}

/* ==========================  成就系统  ========================== */
void checkAchievements() {
    for (auto& achievement : achievements) {
        if (!achievement.unlocked) {
            bool unlocked = false;
            
            if (achievement.name == "初出茅庐") unlocked = (currentUser.totalMining >= 1);
            else if (achievement.name == "抽奖新手") unlocked = (currentUser.totalLottery >= 1);
            else if (achievement.name == "矿工学徒") unlocked = (currentUser.totalMining >= 10);
            else if (achievement.name == "幸运儿") unlocked = (currentUser.totalLottery >= 5);
            else if (achievement.name == "资源大亨") unlocked = (currentUser.money >= 10000);
            else if (achievement.name == "资深矿工") unlocked = (currentUser.totalMining >= 50);
            else if (achievement.name == "宝石收藏家") unlocked = (currentUser.diamonds >= 5);
            else if (achievement.name == "百万富翁") unlocked = (currentUser.money >= 100000);
            else if (achievement.name == "僵尸杀手") unlocked = (currentUser.totalMining >= 50);
            else if (achievement.name == "生存专家") unlocked = (currentUser.depth >= 100);
            else if (achievement.name == "攀登者") unlocked = (currentUser.ladderPlaced);
            else if (achievement.name == "高空作业") unlocked = (currentUser.depth >= 50 && currentUser.ladderPlaced);
            // 新增成就
            else if (achievement.name == "探险家") unlocked = (currentUser.backpackSize >= 24);
            else if (achievement.name == "寻宝者") unlocked = (currentUser.totalMining >= 100 && currentUser.metalDetector > 0);
            else if (achievement.name == "冷静之人") unlocked = (currentUser.totalMining >= 100 && currentUser.fear < 20);
            
            if (unlocked) {
                achievement.unlocked = true;
                
                if (achievement.rewardType == "experience") {
                    currentUser.experience += achievement.rewardValue;
                } else if (achievement.rewardType == "lottery") {
                    currentUser.lotteryCount += achievement.rewardValue;
                } else if (achievement.rewardType == "health") {
                    currentUser.maxHealth += achievement.rewardValue;
                    currentUser.health = currentUser.maxHealth;
                } else if (achievement.rewardType == "diamonds") {
                    currentUser.diamonds += achievement.rewardValue;
                } else if (achievement.rewardType == "backpack") {
                    currentUser.backpackSize += achievement.rewardValue;
                } else if (achievement.rewardType == "healthPotion") {
                    currentUser.healthPotion += achievement.rewardValue;
                }
                
                cout << "\n成就解锁！" << endl;
                cout << "====================" << endl;
                cout << "成就: " << achievement.name << endl;
                cout << "描述: " << achievement.description << endl;
                cout << "奖励: ";
                if (achievement.rewardType == "experience") cout << achievement.rewardValue << "经验";
                else if (achievement.rewardType == "lottery") cout << achievement.rewardValue << "次抽奖机会";
                else if (achievement.rewardType == "health") cout << "最大生命值+" << achievement.rewardValue;
                else if (achievement.rewardType == "diamonds") cout << achievement.rewardValue << "颗钻石";
                else if (achievement.rewardType == "backpack") cout << "背包扩容+" << achievement.rewardValue;
                else if (achievement.rewardType == "healthPotion") cout << achievement.rewardValue << "瓶生命药水";
                cout << endl;
                cout << "====================" << endl;
                Sleep(2000);
            }
        }
    }
}

/* ==========================  每日重置  ========================== */
void checkDailyReset() {
    string today = getCurrentDate();
    if (currentUser.lastLoginDate != today) {
        currentUser.lotteryCount = 0;
        currentUser.lastLoginDate = today;
        currentUser.hunger = min(100, currentUser.hunger + 30);
        currentUser.thirst = min(100, currentUser.thirst + 30);
        currentUser.warmth = min(100, currentUser.warmth + 30);
        currentUser.fear = max(0, currentUser.fear - 30);
    }
}

/* ==========================  物品系统  ========================== */
void showInventory() {
    cout << "\n=== 您的物品库存 ===" << endl;
    cout << "背包容量: " << getTotalItemCount() << "/" << currentUser.backpackSize << endl;
    if (currentUser.storageBox > 0) {
        cout << "地面储物箱: " << currentUser.storageBox << " 个 (只能在地面使用)" << endl;
    }
    cout << "┌──────────────────┬──────────┬───────────┬────────────┐" << endl;
    cout << "│     物品名称     │  数量    │  单价(元) │  总价值(元)│" << endl;
    cout << "├──────────────────┼──────────┼───────────┼────────────┤" << endl;
    
    vector<pair<string, int*>> items = {
        {"钻石", &currentUser.diamonds}, {"蓝宝石", &currentUser.sapphires}, {"绿宝石", &currentUser.emeralds},
        {"黄金", &currentUser.gold}, {"白银", &currentUser.silver}, {"铜", &currentUser.copper},
        {"花岗岩", &currentUser.granite}, {"铁", &currentUser.iron}, {"木头", &currentUser.wood},
        {"煤炭", &currentUser.coal}, {"食物", &currentUser.food}, {"水", &currentUser.water},
        {"绷带", &currentUser.bandage}, {"绊线", &currentUser.trapWire}, {"地雷", &currentUser.mine},
        {"憋气药水", &currentUser.breathPotion}, {"运气药水", &currentUser.luckPotion},
        {"生命药水", &currentUser.healthPotion}, {"抗疲劳药水", &currentUser.fatiguePotion},
        {"夜视药水", &currentUser.nightVisionPotion}, {"力量药水", &currentUser.strengthPotion},
        // 新增
        {"电池", &currentUser.batteries}, {"金属探测仪", &currentUser.metalDetector}
    };
    
    int totalValue = 0;
    for (const auto& item : items) {
        if (*(item.second) > 0 && itemPrices.find(item.first) != itemPrices.end()) {
            int value = *(item.second) * itemPrices[item.first];
            totalValue += value;
            cout << "│ " << setw(16) << left << item.first 
                 << " │ " << setw(8) << *(item.second)
                 << " │ " << setw(9) << itemPrices[item.first]
                 << " │ " << setw(10) << value << " │" << endl;
        }
    }
    
    cout << "├──────────────────┼──────────┼───────────┼────────────┤" << endl;
    cout << "│ " << setw(16) << left << "总价值" 
         << " │ " << setw(8) << ""
         << " │ " << setw(9) << ""
         << " │ " << setw(10) << totalValue << " │" << endl;
    cout << "└──────────────────┴──────────┴───────────┴────────────┘" << endl;
    
    // 显示梯子、金属探测仪等工具
    if (currentUser.ladders > 0 || currentUser.metalDetector > 0) {
        cout << "\n=== 工具 ===" << endl;
        if (currentUser.ladders > 0) cout << "梯子: " << currentUser.ladders << " 个" << endl;
        if (currentUser.metalDetector > 0) cout << "金属探测仪: " << currentUser.metalDetector << " 个 (电池:" << currentUser.batteries << ")" << endl;
    }
}

void useItems() {
    while (true) {
        system("cls");
        centerText(Statistics);
        cout << "=== 使用物品 ===" << endl;
        showStatusBar();
        
        cout << "\n可使用的物品：" << endl;
        cout << "1. 食物 (" << currentUser.food << "个) - 恢复30饥饿值和10温暖值" << endl;
        cout << "2. 水 (" << currentUser.water << "个) - 恢复30口渴值" << endl;
        cout << "3. 绷带 (" << currentUser.bandage << "个) - 恢复20生命值" << endl;
        cout << "4. 憋气药水 (" << currentUser.breathPotion << "个) - 水下呼吸30秒" << endl;
        cout << "5. 运气药水 (" << currentUser.luckPotion << "个) - 下次挖矿稀有物品概率提升" << endl;
        cout << "6. 生命药水 (" << currentUser.healthPotion << "个) - 恢复全部生命值" << endl;
        cout << "7. 抗疲劳药水 (" << currentUser.fatiguePotion << "个) - 恢复50疲惫值" << endl;
        cout << "8. 夜视药水 (" << currentUser.nightVisionPotion << "个) - 夜视效果10分钟" << endl;
        cout << "9. 力量药水 (" << currentUser.strengthPotion << "个) - 挖矿效率提升15分钟" << endl;
        cout << "10. 睡觉 (" << (currentUser.bed > 0 ? "有床" : "无床") << ") - 恢复全部状态" << endl;
        cout << "11. 返回主菜单" << endl;
        
        cout << "\n请选择: ";
        int choice;
        cin >> choice;
        
        if (choice == 11) break;
        
        switch (choice) {
            case 1:
                if (currentUser.food > 0) {
                    currentUser.hunger = min(100, currentUser.hunger + 30);
                    currentUser.warmth = min(100, currentUser.warmth + 10); // 食物增加温暖
                    currentUser.food--;
                    cout << "吃了一个食物，饥饿值恢复30，温暖值恢复10！" << endl;
                } else cout << "没有食物了！" << endl;
                break;
            case 2:
                if (currentUser.water > 0) {
                    currentUser.thirst = min(100, currentUser.thirst + 30);
                    currentUser.water--;
                    cout << "喝了一瓶水，口渴值恢复30！" << endl;
                } else cout << "没有水了！" << endl;
                break;
            case 3:
                if (currentUser.bandage > 0) {
                    currentUser.health = min(currentUser.maxHealth, currentUser.health + 20);
                    currentUser.bandage--;
                    cout << "使用了绷带，生命值恢复20！" << endl;
                } else cout << "没有绷带了！" << endl;
                break;
            case 4:
                if (currentUser.breathPotion > 0) {
                    currentUser.oxygen = 100;
                    currentUser.breathPotion--;
                    cout << "使用了憋气药水，氧气值已恢复！" << endl;
                } else cout << "没有憋气药水了！" << endl;
                break;
            case 5:
                if (currentUser.luckPotion > 0) {
                    currentUser.luckPotion--;
                    cout << "使用了运气药水，下次挖矿运气会更好！" << endl;
                } else cout << "没有运气药水了！" << endl;
                break;
            case 6:
                if (currentUser.healthPotion > 0) {
                    currentUser.health = currentUser.maxHealth;
                    currentUser.healthPotion--;
                    cout << "使用了生命药水，生命值已完全恢复！" << endl;
                } else cout << "没有生命药水了！" << endl;
                break;
            case 7:
                if (currentUser.fatiguePotion > 0) {
                    currentUser.fatigue = min(currentUser.maxFatigue, currentUser.fatigue + 50);
                    currentUser.fatiguePotion--;
                    cout << "使用了抗疲劳药水，疲惫值恢复50！" << endl;
                } else cout << "没有抗疲劳药水了！" << endl;
                break;
            case 8:
                if (currentUser.nightVisionPotion > 0) {
                    currentUser.nightVisionPotion--;
                    cout << "使用了夜视药水，夜晚视野更清晰！" << endl;
                } else cout << "没有夜视药水了！" << endl;
                break;
            case 9:
                if (currentUser.strengthPotion > 0) {
                    currentUser.strengthPotion--;
                    cout << "使用了力量药水，挖矿效率提升！" << endl;
                } else cout << "没有力量药水了！" << endl;
                break;
            case 10:
                if (currentUser.bed > 0 && currentUser.bedDurability > 0) {
                    if (currentUser.depth == 0) {
                        currentUser.health = currentUser.maxHealth;
                        currentUser.fatigue = currentUser.maxFatigue;
                        currentUser.hunger = 100;
                        currentUser.thirst = 100;
                        currentUser.warmth = 100;
                        currentUser.fear = 0;
                        currentUser.bedDurability -= 10;
                        currentUser.isNight = false;
                        currentUser.ladderPlaced = false;
                        currentUser.currentLadderLength = 0;
                        currentUser.currentLadderDurability = 0;
                        cout << "睡了一觉，所有状态已完全恢复！" << endl;
                        if (currentUser.bedDurability <= 0) {
                            currentUser.bed--;
                            cout << "床已损坏！" << endl;
                        }
                    } else {
                        cout << "在矿井中睡觉太危险了！返回地面再睡吧。" << endl;
                    }
                } else {
                    cout << "没有可用的床！" << endl;
                }
                break;
            default:
                cout << "无效选择！" << endl;
                break;
        }
        
        saveUserData();
        cout << "按任意键继续..." << endl;
        cin.get();
        cin.get();
    }
}

/* ==========================  商店系统  ========================== */
void buyItems() {
    vector<pair<string, int>> shopItems = {
        {"钻石", 10000}, {"蓝宝石", 5000}, {"绿宝石", 3000}, {"黄金", 2000},
        {"白银", 800}, {"铜", 300}, {"铁", 500}, {"木头", 100}, {"煤炭", 30},
        {"食物", 50}, {"水", 30}, {"绷带", 80},
        {"憋气药水", 200}, {"运气药水", 300}, {"生命药水", 400}, {"抗疲劳药水", 250},
        {"夜视药水", 350}, {"力量药水", 450}, {"左矿图", 1000}, {"右矿图", 1000},
        {"下矿图", 1500}, {"木床", 500}, {"木镐", 100}, {"石镐", 300}, {"铜镐", 800},
        {"铁镐", 1500}, {"钻石镐", 5000}, {"魔石镐", 10000},
        {"木梯(10米)", 200}, {"木梯(20米)", 350}, {"铁梯(50米)", 1000}, {"钻石梯(无限)", 50000},
        // 新增
        {"背包扩容", 2000}, {"储物箱", 1500}, {"金属探测仪", 3000}, {"电池", 100}
    };
    
    while (true) {
        system("cls");
        centerText(Statistics);
        cout << "=== 购买物品 ===" << endl;
        cout << "当前余额: " << currentUser.money << " 元" << endl;
        cout << "背包容量: " << getTotalItemCount() << "/" << currentUser.backpackSize << endl;
        showStatusBar();
        cout << "\n可购买的物品：" << endl;
        cout << "┌────┬──────────────┬──────────┐" << endl;
        cout << "│编号│    物品名称    │  价格(元) │" << endl;
        cout << "├────┼──────────────┼──────────┤" << endl;
        
        for (int i = 0; i < shopItems.size(); i++) {
            if (shopItems[i].first == "左矿图" && currentUser.mapLeft) continue;
            if (shopItems[i].first == "右矿图" && currentUser.mapRight) continue;
            if (shopItems[i].first == "下矿图" && currentUser.mapDown) continue;
            if (shopItems[i].first == "背包扩容" && currentUser.backpackSize >= 32) continue;
            
            cout << "│ " << setw(2) << right << i+1 
                 << " │ " << setw(12) << left << shopItems[i].first
                 << " │ " << setw(8) << left << shopItems[i].second << " │" << endl;
        }
        cout << "└────┴──────────────┴──────────┘" << endl;
        cout << "\n0. 返回上一级" << endl;
        cout << "请选择要购买的物品编号 (输入0返回): ";
        
        int choice;
        cin >> choice;
        
        if (choice == 0) break;
        if (choice < 1 || choice > shopItems.size()) {
            cout << "无效选择！" << endl;
            Sleep(1000);
            continue;
        }
        
        string itemName = shopItems[choice - 1].first;
        int price = shopItems[choice - 1].second;
        
        if ((itemName == "左矿图" && currentUser.mapLeft) ||
            (itemName == "右矿图" && currentUser.mapRight) ||
            (itemName == "下矿图" && currentUser.mapDown)) {
            cout << "您已经购买过这个矿图了！" << endl;
            Sleep(1000);
            continue;
        }
        
        if (itemName == "背包扩容" && currentUser.backpackSize >= 32) {
            cout << "背包已达到最大容量！" << endl;
            Sleep(1000);
            continue;
        }
        
        // 检查背包空间
        if (itemName != "背包扩容" && itemName != "储物箱" && !hasBackpackSpace(1)) {
            cout << "背包已满！请清理或扩容。" << endl;
            Sleep(1000);
            continue;
        }
        
        cout << "您选择了: " << itemName << endl;
        cout << "价格: " << price << " 元" << endl;
        
        if (price > currentUser.money) {
            cout << "余额不足！" << endl;
            Sleep(1000);
            continue;
        }
        
        cout << "\n确认购买 " << itemName << "？(Y/N): ";
        char confirm;
        cin >> confirm;
        
        if (confirm == 'Y' || confirm == 'y') {
            // 处理梯子
            if (itemName.find("木梯(10米)") != string::npos) currentUser.ladders++;
            else if (itemName.find("木梯(20米)") != string::npos) currentUser.ladders++;
            else if (itemName.find("铁梯(50米)") != string::npos) currentUser.ladders++;
            else if (itemName.find("钻石梯(无限)") != string::npos) currentUser.ladders++;
            // 新增物品
            else if (itemName == "背包扩容") {
                currentUser.backpackSize = min(32, currentUser.backpackSize + 4);
                cout << "背包容量增加到 " << currentUser.backpackSize << " 格！" << endl;
            }
            else if (itemName == "储物箱") {
                currentUser.storageBox++;
                cout << "获得一个储物箱（只能在地面使用）！" << endl;
            }
            else if (itemName == "金属探测仪") currentUser.metalDetector++;
            else if (itemName == "电池") currentUser.batteries += 3;
            // 原有物品
            else if (itemName == "钻石") currentUser.diamonds++;
            else if (itemName == "蓝宝石") currentUser.sapphires++;
            else if (itemName == "绿宝石") currentUser.emeralds++;
            else if (itemName == "黄金") currentUser.gold++;
            else if (itemName == "白银") currentUser.silver++;
            else if (itemName == "铜") currentUser.copper++;
            else if (itemName == "铁") currentUser.iron++;
            else if (itemName == "木头") currentUser.wood++;
            else if (itemName == "煤炭") currentUser.coal++;
            else if (itemName == "食物") currentUser.food++;
            else if (itemName == "水") currentUser.water++;
            else if (itemName == "绷带") currentUser.bandage++;
            // 移除绊线和地雷的购买功能
            else if (itemName == "憋气药水") currentUser.breathPotion++;
            else if (itemName == "运气药水") currentUser.luckPotion++;
            else if (itemName == "生命药水") currentUser.healthPotion++;
            else if (itemName == "抗疲劳药水") currentUser.fatiguePotion++;
            else if (itemName == "夜视药水") currentUser.nightVisionPotion++;
            else if (itemName == "力量药水") currentUser.strengthPotion++;
            else if (itemName == "左矿图") currentUser.mapLeft = true;
            else if (itemName == "右矿图") currentUser.mapRight = true;
            else if (itemName == "下矿图") currentUser.mapDown = true;
            else if (itemName == "木床") { currentUser.bed++; currentUser.bedDurability = 100; }
            else if (itemName == "木镐") { currentUser.woodPickaxe++; currentUser.woodPickaxeDurability = 50; }
            else if (itemName == "石镐") { currentUser.stonePickaxe++; currentUser.stonePickaxeDurability = 100; }
            else if (itemName == "铜镐") { currentUser.copperPickaxe++; currentUser.copperPickaxeDurability = 200; }
            else if (itemName == "铁镐") { currentUser.ironPickaxe++; currentUser.ironPickaxeDurability = 300; }
            else if (itemName == "钻石镐") { currentUser.diamondPickaxe++; currentUser.diamondPickaxeDurability = 500; }
            else if (itemName == "魔石镐") { currentUser.magicPickaxe++; currentUser.magicPickaxeDurability = 1000; }
            
            currentUser.money -= price;
            saveUserData();
            cout << "购买成功！" << endl;
        } else {
            cout << "取消购买。" << endl;
        }
        
        cout << "按任意键继续..." << endl;
        cin.get();
        cin.get();
    }
}

void sellItems() {
    while (true) {
        system("cls");
        centerText(Statistics);
        cout << "=== 卖出物品 ===" << endl;
        showInventory();
        
        cout << "\n可卖出的物品：" << endl;
        vector<pair<string, int*>> sellableItems = {
            {"1. 钻石", &currentUser.diamonds}, {"2. 蓝宝石", &currentUser.sapphires},
            {"3. 绿宝石", &currentUser.emeralds}, {"4. 黄金", &currentUser.gold},
            {"5. 白银", &currentUser.silver}, {"6. 铜", &currentUser.copper},
            {"7. 花岗岩", &currentUser.granite}, {"8. 铁", &currentUser.iron},
            {"9. 木头", &currentUser.wood}, {"10. 煤炭", &currentUser.coal},
            {"11. 食物", &currentUser.food}, {"12. 水", &currentUser.water},
            {"13. 绷带", &currentUser.bandage}, {"14. 绊线", &currentUser.trapWire},
            {"15. 地雷", &currentUser.mine}, {"16. 电池", &currentUser.batteries},
            {"17. 返回上一级", nullptr}
        };
        
        for (const auto& item : sellableItems) {
            if (item.second != nullptr && *item.second > 0) {
                string itemName = item.first.substr(3);
                cout << item.first << " (" << *item.second << "个) - " << itemPrices[itemName] << "元/个" << endl;
            }
        }
        
        cout << "\n请选择要卖出的物品编号: ";
        int choice;
        cin >> choice;
        
        if (choice == 17) break;
        if (choice < 1 || choice > 16) {
            cout << "无效选择！" << endl;
            Sleep(1000);
            continue;
        }
        
        string itemName = sellableItems[choice-1].first.substr(3);
        int* itemCount = sellableItems[choice-1].second;
        int price = itemPrices[itemName];
        
        if (*itemCount <= 0) {
            cout << "您没有" << itemName << "可以卖出！" << endl;
            Sleep(1000);
            continue;
        }
        
        cout << "要卖出多少个？(输入0取消): ";
        int sellCount;
        cin >> sellCount;
        
        if (sellCount <= 0) continue;
        if (sellCount > *itemCount) {
            cout << "您没有那么多" << itemName << "！" << endl;
            Sleep(1000);
            continue;
        }
        
        int totalMoney = sellCount * price;
        *itemCount -= sellCount;
        currentUser.money += totalMoney;
        saveUserData();
        cout << "卖出成功！获得 " << totalMoney << " 元" << endl;
        
        cout << "按任意键继续..." << endl;
        cin.get();
        cin.get();
    }
}

/* ==========================  金属探测仪系统  ========================== */
void useMetalDetector() {
    if (currentUser.metalDetector == 0) {
        cout << "你没有金属探测仪！" << endl;
        return;
    }
    if (currentUser.batteries < 3) {
        cout << "电池不足！需要3个电池。" << endl;
        return;
    }
    
    cout << "正在扫描周围矿脉..." << endl;
    currentUser.batteries -= 3;
    
    // 扫描附近区域
    int foundCount = 0;
    for (const auto& ore : oreDeposits) {
        if (ore.depth == currentUser.depth) {
            int distance = abs(ore.x - currentUser.positionX) + abs(ore.y - currentUser.positionY);
            if (distance <= 15) { // 探测范围
                cout << "发现 " << ore.type << " 矿脉！距离: " << distance << "米" << endl;
                foundCount++;
            }
        }
    }
    
    if (foundCount == 0) {
        cout << "探测范围内未发现矿脉。" << endl;
    } else {
        cout << "总共发现 " << foundCount << " 个矿脉！" << endl;
    }
    
    // 探测仪耐久损耗
    int breakChance = rand() % 10;
    if (breakChance < 2 && currentUser.metalDetector > 0) {
        currentUser.metalDetector--;
        cout << "金属探测仪在扫描中损坏了！" << endl;
    }
    
    Sleep(2000);
}

/* ==========================  挖矿系统  ========================== */
string checkNearbyOres(int direction) {
    for (const auto& ore : oreDeposits) {
        if (ore.depth == currentUser.depth) {
            int distance = 0;
            if (direction == 0 && ore.x == currentUser.positionX && ore.y < currentUser.positionY) {
                distance = currentUser.positionY - ore.y;
                if (distance <= 5) return "下方" + to_string(distance) + "米处有" + ore.type + "矿脉";
            }
            else if (direction == 1 && ore.y == currentUser.positionY && ore.x > currentUser.positionX) {
                distance = ore.x - currentUser.positionX;
                if (distance <= 5) return "右方" + to_string(distance) + "米处有" + ore.type + "矿脉";
            }
            else if (direction == 2 && ore.y == currentUser.positionY && ore.x < currentUser.positionX) {
                distance = currentUser.positionX - ore.x;
                if (distance <= 5) return "左方" + to_string(distance) + "米处有" + ore.type + "矿脉";
            }
        }
    }
    return "未发现矿脉";
}

void encounterZombie() {
    if (zombies.empty() || rand() % 100 >= (15 + currentUser.depth / 8)) return;
    
    cout << "\n警告！附近发现僵尸！" << endl;
    int zombieIndex = rand() % zombies.size();
    Zombie& zombie = zombies[zombieIndex];
    
    cout << "一只" << zombie.type << "向你扑来！" << endl;
    cout << "选择行动: 1.战斗 2.逃跑 3.使用绊线陷阱 4.使用地雷" << endl;
    
    int action;
    cin >> action;
    
    switch (action) {
        case 1:
            if (rand() % 100 < 70) {
                cout << "你成功击退了僵尸！" << endl;
                zombies.erase(zombies.begin() + zombieIndex);
                currentUser.experience += 20;
                currentUser.fear = min(100, currentUser.fear + 10); // 战斗增加恐惧
            } else {
                cout << "你没能击中僵尸，受到了 " << zombie.damage << " 点伤害！" << endl;
                currentUser.health -= zombie.damage;
                currentUser.fear = min(100, currentUser.fear + 20); // 受伤增加更多恐惧
            }
            break;
        case 2:
            if (rand() % 100 < 60) {
                cout << "你成功逃脱了！" << endl;
                currentUser.fear = min(100, currentUser.fear + 5);
            } else {
                cout << "逃跑失败，受到了 " << zombie.damage << " 点伤害！" << endl;
                currentUser.health -= zombie.damage;
                currentUser.fear = min(100, currentUser.fear + 15);
            }
            break;
        case 3:
            if (currentUser.trapWire > 0) {
                currentUser.trapWire--;
                cout << "使用了绊线陷阱，成功困住了僵尸！" << endl;
                zombies.erase(zombies.begin() + zombieIndex);
                currentUser.experience += 25;
                currentUser.fear = max(0, currentUser.fear - 10); // 成功减少恐惧
            } else cout << "没有绊线陷阱了！" << endl;
            break;
        case 4:
            if (currentUser.mine > 0) {
                currentUser.mine--;
                cout << "使用了地雷，炸死了僵尸！" << endl;
                zombies.erase(zombies.begin() + zombieIndex);
                currentUser.experience += 30;
                currentUser.fear = max(0, currentUser.fear - 15);
            } else cout << "没有地雷了！" << endl;
            break;
        default:
            cout << "无效选择，僵尸攻击了你！" << endl;
            currentUser.health -= zombie.damage;
            currentUser.fear = min(100, currentUser.fear + 25);
            break;
    }
    
    currentUser.fatigue -= 10;
    currentUser.hunger -= 5;
    currentUser.thirst -= 5;
    saveUserData();
}

void mining() {
    bool luckBoost = false;
    bool strengthBoost = false;
    
    while (true) {
        system("cls");
        cout << Statistics << endl;
        cout << "=== 挖矿系统 ===" << endl;
        
        checkDailyReset();
        int level = currentUser.experience / 100 + 1;
        int pickaxeType = getCurrentPickaxe();
        
        cout << "时间: " << (currentUser.isNight ? "夜晚" : "白天") << " | 第" << currentUser.dayCount << "天" << endl;
        cout << "当前位置: 深度 " << currentUser.depth << "米, 坐标(" 
             << currentUser.positionX << "," << currentUser.positionY << ")" << endl;
        cout << "当前等级: " << level << " 级" << endl;
        cout << "经验值: " << currentUser.experience << endl;
        cout << "总挖矿次数: " << currentUser.totalMining << " 次" << endl;
        showStatusBar();
        
        if (currentUser.depth > 0) {
            cout << "\n矿图信息:" << endl;
            if (currentUser.mapDown) cout << "↓ " << checkNearbyOres(0) << endl;
            if (currentUser.mapRight) cout << "→ " << checkNearbyOres(1) << endl;
            if (currentUser.mapLeft) cout << "← " << checkNearbyOres(2) << endl;
        }
        
        cout << "\n选择操作：" << endl;
        cout << "1. 向下挖掘 (进入更深层)" << endl;
        cout << "2. 向右挖掘 (X+)" << endl;
        cout << "3. 向左挖掘 (X-)" << endl;
        cout << "4. 使用运气药水 (" << currentUser.luckPotion << "个)" << endl;
        cout << "5. 使用力量药水 (" << currentUser.strengthPotion << "个)" << endl;
        cout << "6. 使用金属探测仪 (" << currentUser.metalDetector << "个, 电池:" << currentUser.batteries << ")" << endl;
            if (currentUser.depth > 0) {
                cout << "7. 使用物品" << endl;
                cout << "8. 放置梯子 (" << currentUser.ladders << "个可用)" << endl;
                cout << "9. 返回地面";
                if (!canReturnToSurface()) cout << " [需要梯子!]";
                cout << endl;
            } else {
                cout << "7. 使用物品" << endl;
                cout << "8. 使用储物箱 (" << currentUser.storageBox << "个可用)" << endl;
                cout << "9. 返回主菜单" << endl;
            }
        cout << "请选择: ";
        
        int choice;
        cin >> choice;
        
        if (currentUser.depth == 0 && choice == 9) break;
        if (currentUser.depth > 0 && choice == 9) {
            if (!canReturnToSurface()) {
                cout << "\n无法返回地面！" << endl;
                if (!currentUser.ladderPlaced) {
                    cout << "你需要先放置梯子！" << endl;
                } else if (currentUser.currentLadderLength < currentUser.depth) {
                    cout << "梯子长度不够！需要至少 " << currentUser.depth << " 米，但你的梯子只有 " 
                         << currentUser.currentLadderLength << " 米。" << endl;
                } else if (currentUser.currentLadderDurability <= 0) {
                    cout << "梯子已经损坏！" << endl;
                }
                Sleep(2000);
            } else {
                // 返回地面，消耗梯子耐久
                if (currentUser.depth > 0 && currentUser.currentLadderLength < 999) {
                    int durabilityLoss = currentUser.depth / 10;
                    currentUser.currentLadderDurability -= durabilityLoss;
                    if (currentUser.currentLadderDurability <= 0) {
                        cout << "梯子在攀爬过程中损坏了！" << endl;
                        currentUser.ladders--;
                        currentUser.ladderPlaced = false;
                    }
                }
                currentUser.depth = currentUser.positionX = currentUser.positionY = 0;
                cout << "已返回地面。" << endl;
                Sleep(1000);
            }
            continue;
        }
        
        if (choice == 6) {
            useMetalDetector();
            saveUserData();
            cin.get();
            cin.get();
            continue;
        }
        
        if (choice == 7) {
            // 使用物品功能
            useItems();
            saveUserData();
            continue;
        }
        
        if (currentUser.depth == 0 && choice == 8) {
            // 使用储物箱
            if (currentUser.storageBox > 0) {
                cout << "\n储物箱只能在地面使用，用于存放物品。" << endl;
                cout << "当前储物箱: " << currentUser.storageBox << " 个" << endl;
                cout << "按任意键继续..." << endl;
                cin.get();
                cin.get();
            } else {
                cout << "没有储物箱！" << endl;
                Sleep(1000);
            }
            continue;
        }
        
        if (currentUser.depth > 0 && choice == 8) {
            // 放置梯子
            if (currentUser.ladders > 0) {
                cout << "\n请选择要放置的梯子：" << endl;
                cout << "1. 10米木梯" << endl;
                cout << "2. 20米木梯" << endl;
                cout << "3. 50米铁梯" << endl;
                cout << "4. 无限钻石梯" << endl;
                cout << "5. 取消" << endl;
                cout << "当前深度: " << currentUser.depth << "米" << endl;
                cout << "选择: ";
                
                int ladderChoice;
                cin >> ladderChoice;
                
                if (ladderChoice >= 1 && ladderChoice <= 4) {
                    currentUser.ladders--;
                    currentUser.ladderPlaced = true;
                    
                    switch(ladderChoice) {
                        case 1:
                            currentUser.currentLadderLength = 10;
                            currentUser.currentLadderDurability = 100;
                            break;
                        case 2:
                            currentUser.currentLadderLength = 20;
                            currentUser.currentLadderDurability = 100;
                            break;
                        case 3:
                            currentUser.currentLadderLength = 50;
                            currentUser.currentLadderDurability = 150;
                            break;
                        case 4:
                            currentUser.currentLadderLength = 999;
                            currentUser.currentLadderDurability = 999;
                            break;
                    }
                    cout << "成功放置了梯子！" << endl;
                    saveUserData();
                    Sleep(1000);
                }
            } else {
                cout << "没有可用的梯子！" << endl;
                Sleep(1000);
            }
            continue;
        }
        
        if (choice == 4) {
            if (currentUser.luckPotion > 0) {
                luckBoost = true;
                currentUser.luckPotion--;
                cout << "使用了运气药水！" << endl;
            } else cout << "没有运气药水了！" << endl;
            Sleep(1000);
            continue;
        }
        
        if (choice == 5) {
            if (currentUser.strengthPotion > 0) {
                strengthBoost = true;
                currentUser.strengthPotion--;
                cout << "使用了力量药水！" << endl;
            } else cout << "没有力量药水了！" << endl;
            Sleep(1000);
            continue;
        }
        
        if (choice < 1 || choice > 3) {
            cout << "无效选择！" << endl;
            Sleep(1000);
            continue;
        }
        
        if (getCurrentPickaxeDurability() <= 0) {
            cout << "镐子已损坏，无法挖矿！" << endl;
            Sleep(1000);
            continue;
        }
        
        // 检查是否有可用的镐子
        bool hasPickaxe = false;
        if ((currentUser.woodPickaxe > 0 && currentUser.woodPickaxeDurability > 0) ||
            (currentUser.stonePickaxe > 0 && currentUser.stonePickaxeDurability > 0) ||
            (currentUser.copperPickaxe > 0 && currentUser.copperPickaxeDurability > 0) ||
            (currentUser.ironPickaxe > 0 && currentUser.ironPickaxeDurability > 0) ||
            (currentUser.diamondPickaxe > 0 && currentUser.diamondPickaxeDurability > 0) ||
            (currentUser.magicPickaxe > 0 && currentUser.magicPickaxeDurability > 0)) {
            hasPickaxe = true;
        }
        
        if (!hasPickaxe) {
            cout << "没有镐头不能挖矿！请先购买镐子。" << endl;
            Sleep(1000);
            continue;
        }
        
        if (currentUser.fatigue < 10 || currentUser.hunger < 10 || currentUser.thirst < 10) {
            cout << "状态过低，无法挖矿！" << endl;
            Sleep(1000);
            continue;
        }
        
        // 开始挖矿
        string direction;
        switch (choice) {
            case 1: direction = "下方"; currentUser.depth++; currentUser.positionY--; break;
            case 2: direction = "右方"; currentUser.positionX++; break;
            case 3: direction = "左方"; currentUser.positionX--; break;
        }
        
        // 恐惧值影响挖矿速度
        int miningSpeed = 300;
        if (strengthBoost) miningSpeed = 150;
        if (currentUser.fear > 60) miningSpeed += 100; // 恐惧降低效率
        
        cout << "\n向" << direction << "挖掘中";
        for (int i = 0; i < 3; i++) {
            cout << "■";
            Sleep(miningSpeed);
        }
        cout << endl;
        
        // 消耗资源
        currentUser.totalMining++;
        int expGain = 3 + level/2;
        currentUser.experience += expGain;
        currentUser.fatigue -= 8 + currentUser.fear/20; // 恐惧增加疲劳消耗
        currentUser.hunger -= 5;
        currentUser.thirst -= 3;
        currentUser.warmth -= 2 + currentUser.depth/100; // 深处更冷
        currentUser.fear = min(100, currentUser.fear + 2 + currentUser.depth/200); // 深处更恐怖
        usePickaxeDurability();
        updateTime();
        
        // 生成僵尸
        generateZombies();
        
        // 挖矿逻辑
        int baseChance = 1000;
        if (luckBoost) baseChance += 200;
        if (pickaxeType >= 4) baseChance += 100;
        if (currentUser.isNight) baseChance -= 100;
        
        int randomValue = rand() % baseChance;
        string foundItem;
        int amount = 1;
        
        if (randomValue < 300) { foundItem = "花岗岩"; amount = rand() % 3 + 1; }
        else if (randomValue < 500) { foundItem = "煤炭"; amount = rand() % 5 + 2; }
        else if (randomValue < 650) { foundItem = "铁"; amount = rand() % 3 + 1; }
        else if (randomValue < 750) { foundItem = "铜"; amount = rand() % 2 + 1; }
        else if (randomValue < 830) { foundItem = "银"; amount = rand() % 2 + 1; }
        else if (randomValue < 880) { foundItem = "木头"; amount = rand() % 4 + 2; }
        else if (randomValue < 905) { foundItem = "黄金"; amount = rand() % 2 + 1; }
        else if (randomValue < 913) { foundItem = "钻石"; }
        else if (randomValue < 918) { foundItem = "蓝宝石"; }
        else if (randomValue < 923) { foundItem = "绿宝石"; }
        else if (randomValue < 928) { foundItem = "抽奖次数"; currentUser.lotteryCount++; }
        else if (randomValue < 935) { foundItem = "电池"; currentUser.batteries++; } // 新增
        else if (randomValue < 940) { foundItem = "绷带"; currentUser.bandage++; }
        else if (randomValue < 950) { foundItem = "绊线"; currentUser.trapWire++; }
        else if (randomValue < 955) { foundItem = "魔矿石"; }
        else { foundItem = "金钱"; amount = rand() % 20 + 10; currentUser.money += amount; }
        
        // 检查背包空间
        if (!hasBackpackSpace(amount)) {
            cout << "背包已满！无法获得 " << foundItem << endl;
            cout << "多余物品已丢失！" << endl;
            // 不增加物品
        } else {
            // 更新库存
            if (foundItem == "钻石") currentUser.diamonds += amount;
            else if (foundItem == "蓝宝石") currentUser.sapphires += amount;
            else if (foundItem == "绿宝石") currentUser.emeralds += amount;
            else if (foundItem == "黄金") currentUser.gold += amount;
            else if (foundItem == "银") currentUser.silver += amount;
            else if (foundItem == "铜") currentUser.copper += amount;
            else if (foundItem == "铁") currentUser.iron += amount;
            else if (foundItem == "煤炭") currentUser.coal += amount;
            else if (foundItem == "花岗岩") currentUser.granite += amount;
            else if (foundItem == "木头") currentUser.wood += amount;
            else if (foundItem == "电池") currentUser.batteries += amount;
            else if (foundItem == "魔矿石") { 
                currentUser.magicPickaxeDurability = min(1000, currentUser.magicPickaxeDurability + 100);
                cout << "发现魔矿石，魔石镐耐久度+100！" << endl;
            }
        }
        
        cout << "\n挖矿结果！" << endl;
        cout << "=================================" << endl;
        cout << "获得了 " << amount << " 个" << foundItem << "！" << endl;
        cout << "经验值 +" << expGain << endl;
        if (foundItem == "金钱") cout << "金钱 +" << amount << endl;
        cout << "=================================" << endl;
        
        // 遭遇事件
        int eventChance = rand() % 100;
        if (eventChance < 12) {
            cout << "挖到了地下水！" << endl;
            if (currentUser.breathPotion > 0) {
                cout << "自动使用憋气药水..." << endl;
                currentUser.oxygen = 100;
                currentUser.breathPotion--;
            } else {
                for (int i = 5; i > 0 && currentUser.oxygen > 0; i--) {
                    cout << "氧气剩余: " << i << "秒" << endl;
                    currentUser.oxygen -= 20;
                    Sleep(1000);
                }
            }
        }
        else if (eventChance < 25) {
            encounterZombie();
        }
        else if (eventChance < 30) {
            cout << "听到了奇怪的声音..." << endl;
            currentUser.fear = min(100, currentUser.fear + 15);
        }
        else if (eventChance < 35) {
            cout << "幸运！你发现了一个隐藏的宝藏箱！" << endl;
            int bonus = rand() % 300 + 100;
            currentUser.money += bonus;
            cout << "获得额外 " << bonus << " 元！" << endl;
            currentUser.fear = max(0, currentUser.fear - 10); // 宝藏减少恐惧
        }
        
        luckBoost = false;
        strengthBoost = false;
        saveUserData();
        checkAchievements();
        
        if (!checkSurvival()) break;
        
        cout << "\n按任意键继续..." << endl;
        cin.get();
        cin.get();
    }
}

/* ==========================  抽奖系统  ========================== */
void lottery() {
    system("cls");
    cout << Statistics << endl;
    
    checkDailyReset();
    
    if (currentUser.lotteryCount >= 5) {
        cout << "今日抽奖次数已用完！" << endl;
        cout << "按任意键继续..." << endl;
        cin.get();
        cin.get();
        return;
    }
    
    cout << "今日剩余抽奖次数: " << 5 - currentUser.lotteryCount << endl;
    cout << "开始抽奖？(Y/N): ";
    char choice;
    cin >> choice;
    
    if (choice == 'Y' || choice == 'y') {
        currentUser.lotteryCount++;
        currentUser.totalLottery++;
        
        cout << "抽奖中";
        for (int i = 0; i < 3; i++) {
            cout << ".";
            Sleep(500);
        }
        cout << endl;
        
        int randomNum = rand() % 100 + 1;
        
        if (randomNum <= 5) {
            cout << "恭喜！您已获得钻石*1" << endl;
            currentUser.diamonds++;
        } else if (randomNum <= 10) {
            cout << "恭喜！您已获得蓝宝石*1" << endl;
            currentUser.sapphires++;
        } else if (randomNum <= 15) {
            cout << "恭喜！您已获得绿宝石*1" << endl;
            currentUser.emeralds++;
        } else if (randomNum <= 20) {
            cout << "恭喜！您已获得1000元" << endl;
            currentUser.money += 1000;
        } else if (randomNum <= 25) {
            cout << "恭喜！您已获得黄金2克" << endl;
            currentUser.gold += 2;
        } else if (randomNum <= 40) {
            cout << "恭喜！您已获得花岗岩*3" << endl;
            currentUser.granite += 3;
        } else if (randomNum <= 50) {
            cout << "恭喜！您已获得经验值100" << endl;
            currentUser.experience += 100;
        } else if (randomNum <= 65) {
            cout << "恭喜！您已获得铁*2" << endl;
            currentUser.iron += 2;
        } else if (randomNum <= 80) {
            cout << "恭喜！您已获得木头*3" << endl;
            currentUser.wood += 3;
        } else if (randomNum <= 90) {
            cout << "恭喜！您已获得食物*2" << endl;
            currentUser.food += 2;
        } else if (randomNum <= 95) {
            cout << "恭喜！您已获得水*3" << endl;
            currentUser.water += 3;
        } else if (randomNum <= 98) {
            cout << "恭喜！您获得了抽奖机会+1！" << endl;
            currentUser.lotteryCount = min(4, currentUser.lotteryCount - 1); // 减少已用次数，相当于增加一次抽奖机会
        } else {
            cout << "抱歉，您没有抽中任何物品" << endl;
        }
        
        saveUserData();
        checkAchievements();
    }
    
    cout << "按任意键继续..." << endl;
    cin.get();
    cin.get();
}

/* ==========================  信息显示  ========================== */
void showUserInfo() {
    system("cls");
    cout << Statistics << endl;
    cout << "=== 用户信息 ===" << endl;
    cout << "用户名: " << currentUser.username << endl;
    cout << "注册日期: " << currentUser.registerDate << endl;
    cout << "最后登录: " << currentUser.lastLoginDate << endl;
    cout << "时间: " << (currentUser.isNight ? "夜晚" : "白天") << " | 第" << currentUser.dayCount << "天" << endl;
    cout << "金钱: " << currentUser.money << " 元" << endl;
    cout << "经验值: " << currentUser.experience << endl;
    cout << "等级: " << (currentUser.experience / 100 + 1) << endl;
    cout << "背包容量: " << currentUser.backpackSize << " 格" << endl;
    showStatusBar();
    cout << "\n=== 统计数据 ===" << endl;
    cout << "总挖矿次数: " << currentUser.totalMining << " 次" << endl;
    cout << "总抽奖次数: " << currentUser.totalLottery << " 次" << endl;
    cout << "当前位置: 深度 " << currentUser.depth << "米" << endl;
    cout << "\n=== 物品库存 ===" << endl;
    showInventory();
    cout << "\n按任意键继续..." << endl;
    cin.get();
    cin.get();
}

void showAchievements() {
    system("cls");
    cout << Statistics << endl;
    cout << "=== 成就系统 ===" << endl;
    
    int unlockedCount = 0;
    for (const auto& achievement : achievements) {
        if (achievement.unlocked) unlockedCount++;
    }
    
    cout << "成就进度: " << unlockedCount << "/" << achievements.size() << endl;
    cout << "┌──────────────────┬─────────────────────────────┬────────────┬──────────┐" << endl;
    cout << "│     成就名称     │           描述             │    状态    │   奖励   │" << endl;
    cout << "├──────────────────┼─────────────────────────────┼────────────┼──────────┤" << endl;
    
    for (const auto& achievement : achievements) {
        cout << "│ " << setw(16) << left << achievement.name
             << " │ " << setw(27) << left << achievement.description
             << " │ " << setw(10) << left << (achievement.unlocked ? "已解锁" : "未解锁")
             << " │ ";
        
        if (achievement.unlocked) {
            if (achievement.rewardType == "experience") cout << setw(8) << achievement.rewardValue << "经验";
            else if (achievement.rewardType == "lottery") cout << setw(8) << achievement.rewardValue << "抽奖";
            else if (achievement.rewardType == "health") cout << setw(8) << "生命+" << achievement.rewardValue;
            else if (achievement.rewardType == "diamonds") cout << setw(8) << achievement.rewardValue << "钻石";
            else if (achievement.rewardType == "backpack") cout << setw(8) << "背包+" << achievement.rewardValue;
            else if (achievement.rewardType == "healthPotion") cout << setw(8) << achievement.rewardValue << "药水";
        } else {
            cout << setw(8) << "???";
        }
        cout << " │" << endl;
    }
    cout << "└──────────────────┴─────────────────────────────┴────────────┴──────────┘" << endl;
    
    cout << "\n按任意键继续..." << endl;
    cin.get();
    cin.get();
}

void showLeaderboard() {
    system("cls");
    cout << Statistics << endl;
    cout << "=== 排行榜 ===" << endl;
    cout << "（基于经验值排名）" << endl;
    cout << "┌────┬──────────────────┬──────────┬──────────┬────────────┐" << endl;
    cout << "│排名│      用户名      │   经验值  │   等级   │   总财富   │" << endl;
    cout << "├────┼──────────────────┼──────────┼──────────┼────────────┤" << endl;
    
    int totalWealth = currentUser.money;
    for (const auto& item : itemPrices) {
        if (item.first == "钻石") totalWealth += currentUser.diamonds * item.second;
        else if (item.first == "蓝宝石") totalWealth += currentUser.sapphires * item.second;
        else if (item.first == "绿宝石") totalWealth += currentUser.emeralds * item.second;
        else if (item.first == "黄金") totalWealth += currentUser.gold * item.second;
        else if (item.first == "白银") totalWealth += currentUser.silver * item.second;
        else if (item.first == "铜") totalWealth += currentUser.copper * item.second;
        else if (item.first == "花岗岩") totalWealth += currentUser.granite * item.second;
        else if (item.first == "铁") totalWealth += currentUser.iron * item.second;
        else if (item.first == "木头") totalWealth += currentUser.wood * item.second;
        else if (item.first == "煤炭") totalWealth += currentUser.coal * item.second;
    }
    
    cout << "│ " << setw(2) << right << 1 
         << " │ " << setw(16) << left << currentUser.username
         << " │ " << setw(8) << right << currentUser.experience
         << " │ " << setw(8) << right << (currentUser.experience / 100 + 1)
         << " │ " << setw(10) << right << totalWealth << " │" << endl;
    
    cout << "└────┴──────────────────┴──────────┴──────────┴────────────┘" << endl;
    cout << "\n提示：目前只显示当前用户" << endl;
    cout << "\n按任意键继续..." << endl;
    cin.get();
    cin.get();
}

/* ==========================  控制台系统  ========================== */
void debugConsole() {
    system("cls");
    cout << "=== 游戏控制台 ===" << endl;
    cout << "警告：使用控制台命令可能影响游戏体验！" << endl;
    cout << "提示：输入 'help' 查看所有可用命令" << endl;

    string line;
    while (true) {
        cout << "\nStatistics Command> ";
        if (!getline(cin, line)) break;

        string cmd = line;
        transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if (cmd == "exit" || cmd == "quit" || cmd == "q") break;

        if (cmd == "help") {
            cout << "money [数量]      - 增加金钱\n"
                "exp   [数量]      - 增加经验\n"
                "heal              - 恢复所有状态\n"
                "items             - 获得所有物品各10个\n"
                "achievements      - 解锁所有成就\n"
                "keepinv           - 切换死亡不掉落\n"
                "surface           - 传送到地面\n"
                "lottery [数量]    - 增加抽奖次数\n"
                "additem <名> [数] - 增加特定物品\n"
                "max               - 最大化所有资源\n"
                "map               - 解锁所有地图\n"
                "ladder [数]       - 增加梯子\n"
                "fear [值]         - 设置恐惧值\n"
                "warmth [值]       - 设置温暖值\n"
                "backpack [值]     - 设置背包大小\n"
                "quickstart [1/0]  - 设置快速启动\n"
                "save              - 保存游戏\n"
                "exit/quit/q       - 返回主菜单\n";
        }
        else if (cmd.rfind("money", 0) == 0) {
            int amt = 10000;
            sscanf(line.c_str(), "money %d", &amt);
            currentUser.money += amt;
            cout << "→ 增加了 " << amt << " 金币\n";
        }
        else if (cmd.rfind("exp", 0) == 0) {
            int amt = 1000;
            sscanf(line.c_str(), "exp %d", &amt);
            currentUser.experience += amt;
            cout << "→ 增加了 " << amt << " 经验\n";
        }
        else if (cmd == "heal") {
            currentUser.health = currentUser.maxHealth;
            currentUser.fatigue = currentUser.maxFatigue;
            currentUser.hunger = currentUser.thirst = currentUser.oxygen = 100;
            currentUser.warmth = 100;
            currentUser.fear = 0;
            cout << "→ 所有状态已完全恢复\n";
        }
        else if (cmd.rfind("quickstart", 0) == 0) {
            bool enable;
            sscanf(line.c_str(), "quickstart %d", &enable);
            // 保存快速启动设置
              gameConfig.quickStart = enable;
              saveGameConfig();
            cout << "→ 快速启动功能已" << (gameConfig.quickStart ? "开启" : "关闭") << "\n";
            cout << "→ 重启游戏后生效\n";
        }
        else if (cmd == "items") {
            currentUser.diamonds += 10;
            currentUser.sapphires += 10;
            currentUser.emeralds += 10;
            currentUser.gold += 10;
            currentUser.silver += 10;
            currentUser.copper += 10;
            currentUser.iron += 10;
            currentUser.wood += 10;
            currentUser.coal += 10;
            currentUser.granite += 10;
            currentUser.food += 10;
            currentUser.water += 10;
            currentUser.bandage += 10;
            currentUser.trapWire += 10;
            currentUser.mine += 10;
            currentUser.breathPotion += 10;
            currentUser.luckPotion += 10;
            currentUser.healthPotion += 10;
            currentUser.fatiguePotion += 10;
            currentUser.nightVisionPotion += 10;
            currentUser.strengthPotion += 10;
            currentUser.batteries += 10;
            currentUser.metalDetector += 1;
            cout << "→ 获得了所有物品各10个\n";
        }
        else if (cmd == "achievements") {
            for (auto &a : achievements) a.unlocked = true;
            cout << "→ 解锁了所有成就\n";
        }
        else if (cmd == "keepinv") {
            currentUser.keepInventory = !currentUser.keepInventory;
            cout << "→ 死亡不掉落: " << (currentUser.keepInventory ? "开启" : "关闭") << '\n';
        }
        else if (cmd == "surface") {
            currentUser.depth = currentUser.positionX = currentUser.positionY = 0;
            cout << "→ 已传送至地面\n";
        }
        else if (cmd.rfind("lottery", 0) == 0) {
            int amt = 1;
            sscanf(line.c_str(), "lottery %d", &amt);
            currentUser.lotteryCount += amt;
            cout << "→ 增加了 " << amt << " 次抽奖机会\n";
        }
        else if (cmd.rfind("ladder", 0) == 0) {
            int amt = 1;
            sscanf(line.c_str(), "ladder %d", &amt);
            currentUser.ladders += amt;
            cout << "→ 增加了 " << amt << " 个梯子\n";
        }
        else if (cmd.rfind("fear", 0) == 0) {
            int amt = 0;
            sscanf(line.c_str(), "fear %d", &amt);
            currentUser.fear = max(0, min(100, amt));
            cout << "→ 恐惧值设置为 " << currentUser.fear << '\n';
        }
        else if (cmd.rfind("warmth", 0) == 0) {
            int amt = 100;
            sscanf(line.c_str(), "warmth %d", &amt);
            currentUser.warmth = max(0, min(100, amt));
            cout << "→ 温暖值设置为 " << currentUser.warmth << '\n';
        }
        else if (cmd.rfind("backpack", 0) == 0) {
            int amt = 16;
            sscanf(line.c_str(), "backpack %d", &amt);
            currentUser.backpackSize = max(16, min(64, amt));
            cout << "→ 背包容量设置为 " << currentUser.backpackSize << '\n';
        }
        else if (cmd.rfind("additem", 0) == 0) {
            char name[32];
            int amt = 1;
            sscanf(line.c_str(), "additem %s %d", name, &amt);
            string item(name);
            transform(item.begin(), item.end(), item.begin(), ::tolower);

            auto add = [&](int &field) {
                field += amt;
                cout << "→ 成功添加 " << amt << " 个 " << item << '\n';
            };
            if (item == "钻石" || item == "diamond") add(currentUser.diamonds);
            else if (item == "蓝宝石" || item == "sapphire") add(currentUser.sapphires);
            else if (item == "绿宝石" || item == "emerald") add(currentUser.emeralds);
            else if (item == "黄金" || item == "gold") add(currentUser.gold);
            else if (item == "白银" || item == "silver") add(currentUser.silver);
            else if (item == "铜" || item == "copper") add(currentUser.copper);
            else if (item == "铁" || item == "iron") add(currentUser.iron);
            else if (item == "木头" || item == "wood") add(currentUser.wood);
            else if (item == "煤炭" || item == "coal") add(currentUser.coal);
            else if (item == "花岗岩" || item == "granite") add(currentUser.granite);
            else if (item == "食物" || item == "food") add(currentUser.food);
            else if (item == "水" || item == "water") add(currentUser.water);
            else if (item == "绷带" || item == "bandage") add(currentUser.bandage);
            else if (item == "绊线" || item == "trapwire") add(currentUser.trapWire);
            else if (item == "地雷" || item == "mine") add(currentUser.mine);
            else if (item == "电池" || item == "battery") add(currentUser.batteries);
            else if (item == "金属探测仪" || item == "detector") add(currentUser.metalDetector);
            else if (item == "憋气药水" || item == "breathpotion") add(currentUser.breathPotion);
            else if (item == "运气药水" || item == "luckpotion") add(currentUser.luckPotion);
            else if (item == "生命药水" || item == "healthpotion") add(currentUser.healthPotion);
            else if (item == "抗疲劳药水" || item == "fatiguepotion") add(currentUser.fatiguePotion);
            else if (item == "夜视药水" || item == "nightvisionpotion") add(currentUser.nightVisionPotion);
            else if (item == "力量药水" || item == "strengthpotion") add(currentUser.strengthPotion);
            else cout << "→ 未知物品：" << item << '\n';
        }
        else if (cmd == "max") {
            currentUser.money = 999999;
            currentUser.experience = 99999;
            currentUser.diamonds = currentUser.sapphires = currentUser.emeralds = 999;
            currentUser.gold = currentUser.silver = currentUser.copper = 999;
            currentUser.iron = currentUser.wood = currentUser.coal = currentUser.granite = 999;
            currentUser.food = currentUser.water = currentUser.bandage = 99;
            currentUser.trapWire = currentUser.mine = 99;
            currentUser.breathPotion = currentUser.luckPotion = currentUser.healthPotion = currentUser.fatiguePotion = 99;
            currentUser.nightVisionPotion = currentUser.strengthPotion = 99;
            currentUser.batteries = 99;
            currentUser.metalDetector = 5;
            currentUser.lotteryCount = 99;
            currentUser.ladders = 10;
            cout << "→ 所有资源已最大化\n";
        }
        else if (cmd == "map") {
            currentUser.mapLeft = currentUser.mapRight = currentUser.mapDown = true;
            cout << "→ 所有地图已解锁\n";
        }
        else if (cmd == "save") {
            saveUserData();
            cout << "→ 游戏已保存\n";
        }
        else if (cmd == "allowshop") {
            // 允许在挖矿时使用商店的特殊命令（仅管理员可用）
            if (currentUser.username == "Administrator") {
                cout << "→ 特殊模式：挖矿时允许使用商店\n";
                // 直接调用商店函数，绕过深度检查
                system("cls");
                centerText(Statistics);
                cout << "=== 商店（特殊模式） ===" << endl;
                cout << "A.购买物品   B.卖出物品   C.查看库存   D.使用物品   E.返回控制台" << endl;
                cout << "当前余额: " << currentUser.money << " 元" << endl;
                showStatusBar();
                cout << "请选择: ";
                
                char choice;
                cin >> choice;
                
                if (choice == 'A' || choice == 'a') buyItems();
                else if (choice == 'B' || choice == 'b') sellItems();
                else if (choice == 'C' || choice == 'c') {
                    system("cls");
                centerText(Statistics);
                showInventory();
                    cout << "\n按任意键继续..." << endl;
                    cin.get();
                    cin.get();
                }
                else if (choice == 'D' || choice == 'd') useItems();
                cout << "→ 返回控制台\n";
            } else {
                cout << "→ 权限不足，只有管理员可以使用此命令\n";
            }
        }
        else {
            cout << "→ 未知命令，输入 help 查看\n";
        }
    }
}

/* ==========================  商店系统  ========================== */
void shop() {
    // 检查是否在挖矿状态（深度大于0）
    if (currentUser.depth > 0) {
        cout << "在挖矿时无法访问商店！请先返回地面。" << endl;
        Sleep(2000);
        return;
    }
    
    while (true) {
        system("cls");
        centerText(Statistics);
        cout << "=== 商店 ===" << endl;
        cout << "A.购买物品   B.卖出物品   C.查看库存   D.使用物品   E.返回主菜单" << endl;
        cout << "当前余额: " << currentUser.money << " 元" << endl;
        showStatusBar();
        cout << "请选择: ";
        
        char choice;
        cin >> choice;
        
        if (choice == 'A' || choice == 'a') buyItems();
        else if (choice == 'B' || choice == 'b') sellItems();
        else if (choice == 'C' || choice == 'c') {
            system("cls");
            cout << Statistics << endl;
            showInventory();
            cout << "\n按任意键继续..." << endl;
            cin.get();
            cin.get();
        }
        else if (choice == 'D' || choice == 'd') useItems();
        else if (choice == 'E' || choice == 'e') break;
        else {
            cout << "无效选择！" << endl;
            Sleep(1000);
        }
    }
}

/* ==========================  设置系统  ========================== */
void settings() {
    while (true) {
        system("cls");
        centerText(Statistics);
        cout << "=== 设置 ===" << endl;
        cout << "1. 查看游戏说明" << endl;
        cout << "2. 查看成就" << endl;
        cout << "3. 查看排行榜" << endl;
        cout << "4. 控制台(调试)" << endl;
        cout << "5. 重置游戏数据" << endl;
        cout << "6. 退出登录" << endl;
        cout << "7. 返回主菜单" << endl;
        cout << "请选择: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 1) {
            cout << "\n=== 游戏说明 ===" << endl;
            cout << "- 每日可抽奖5次，挖矿无限制" << endl;
            cout << "- 数据会自动保存" << endl;
            cout << "- 等级越高，挖矿效率越高" << endl;
            cout << "- 可以在商店购买和卖出物品" << endl;
            cout << "- 挖矿可以获得各种矿石、金钱和抽奖次数" << endl;
            cout << "- 经验值可以提升等级" << endl;
            cout << "- 完成成就获得丰厚奖励" << endl;
            cout << "\n=== 生存系统 ===" << endl;
            cout << "- 生命值、疲惫值、饥饿值、口渴值、温暖值、恐惧值" << endl;
            cout << "- 食物恢复饥饿和温暖" << endl;
            cout << "- 夜晚和深处会增加恐惧和降低温暖" << endl;
            cout << "\n=== 背包系统 ===" << endl;
            cout << "- 背包最多容纳16种物品" << endl;
            cout << "- 购买背包扩容可增加容量" << endl;
            cout << "- 储物箱只能在地面使用" << endl;
            cout << "\n=== 梯子系统 ===" << endl;
            cout << "- 深矿需要梯子才能返回" << endl;
            cout << "- 梯子有多种类型：木梯、铁梯、钻石梯" << endl;
            cout << "- 放置梯子后，梯子长度必须大于等于深度" << endl;
            cout << "- 爬梯会消耗梯子耐久度" << endl;
            cout << "\n=== 金属探测仪 ===" << endl;
            cout << "- 需要3个电池使用" << endl;
            cout << "- 可探测附近矿脉位置" << endl;
            cout << "- 有损耗风险" << endl;
            cout << "\n按任意键继续..." << endl;
            cin.get();
            cin.get();
        } else if (choice == 2) showAchievements();
        else if (choice == 3) showLeaderboard();
        else if (choice == 4) debugConsole();
        else if (choice == 5) {
            cout << "确定要重置所有数据吗？(Y/N): ";
            char confirm;
            cin >> confirm;
            if (confirm == 'Y' || confirm == 'y') {
                UserData newUser;
                newUser.username = currentUser.username;
                newUser.password = currentUser.password;
                newUser.registerDate = currentUser.registerDate;
                newUser.lastLoginDate = getCurrentDate();
                newUser.health = 100;
                newUser.maxHealth = 100;
                newUser.fatigue = 100;
                newUser.maxFatigue = 100;
                newUser.hunger = 100;
                newUser.thirst = 100;
                newUser.warmth = 100;
                newUser.fear = 0;
                newUser.oxygen = 100;
                newUser.food = 5;
                newUser.water = 5;
                newUser.bandage = 3;
                newUser.trapWire = 5;
                newUser.mine = 2;
                newUser.woodPickaxe = 1;
                newUser.woodPickaxeDurability = 50;
                newUser.ladders = 0;
                newUser.ladderPlaced = false;
                newUser.currentLadderLength = 0;
                newUser.currentLadderDurability = 0;
                newUser.backpackSize = 16; // 保留背包扩容
                newUser.storageBox = 0;
                newUser.metalDetector = 0;
                newUser.batteries = 0;
                currentUser = newUser;
                saveUserData();
                cout << "数据重置成功！" << endl;
                Sleep(1000);
            }
        } else if (choice == 6) {
            cout << "确定要退出登录吗？(Y/N): ";
            char confirm;
            cin >> confirm;
            if (confirm == 'Y' || confirm == 'y') {
                cout << "退出登录中..." << endl;
                Sleep(1000);
                return;
            }
        } else if (choice == 7) break;
        else {
            cout << "无效选择！" << endl;
            Sleep(1000);
        }
    }
}

/* ==========================  登录系统  ========================== */
bool registerUser() {
    string username, password, confirmPassword;
    cout << "=== 用户注册 ===" << endl;
    cout << "请输入用户名: ";
    cin >> username;
    
    ifstream testFile(username + ".dat");
    if (testFile.is_open()) {
        cout << "用户名已存在！" << endl;
        testFile.close();
        Sleep(1000);
        return false;
    }
    
    cout << "请输入密码: ";
    cin >> password;
    cout << "请确认密码: ";
    cin >> confirmPassword;
    
    if (password != confirmPassword) {
        cout << "密码不一致！" << endl;
        Sleep(1000);
        return false;
    }
    
    if (password.length() < 3) {
        cout << "密码长度至少3位！" << endl;
        Sleep(1000);
        return false;
    }
    
    currentUser.username = username;
    currentUser.password = password;
    currentUser.registerDate = getCurrentDate();
    currentUser.lastLoginDate = getCurrentDate();
    currentUser.health = 100;
    currentUser.maxHealth = 100;
    currentUser.fatigue = 100;
    currentUser.maxFatigue = 100;
    currentUser.hunger = 100;
    currentUser.thirst = 100;
    currentUser.warmth = 100;
    currentUser.fear = 0;
    currentUser.oxygen = 100;
    currentUser.food = 5;
    currentUser.water = 5;
    currentUser.bandage = 3;
    currentUser.trapWire = 5;
    currentUser.mine = 2;
    currentUser.woodPickaxe = 1;
    currentUser.woodPickaxeDurability = 50;
    currentUser.ladders = 0;
    currentUser.ladderPlaced = false;
    currentUser.currentLadderLength = 0;
    currentUser.currentLadderDurability = 0;
    currentUser.backpackSize = 16;
    currentUser.storageBox = 0;
    currentUser.metalDetector = 0;
    currentUser.batteries = 0;
    
    generateOreDeposits();
    saveUserData();
    cout << "注册成功！自动登录..." << endl;
    Sleep(1000);
    return true;
}

// 加载游戏配置
void loadGameConfig() {
    ifstream configFile("config.ini");
    if (configFile.is_open()) {
        string line;
        bool foundConfig = false;
        
        // 查找配置区块
        while (getline(configFile, line)) {
            if (line == "[GameConfig]") {
                foundConfig = true;
                break;
            }
        }
        
        if (foundConfig) {
            map<string, string> data;
            while (getline(configFile, line) && line[0] != '[') {
                if (line.empty()) continue;
                size_t pos = line.find('=');
                if (pos != string::npos) {
                    string key = line.substr(0, pos);
                    string value = line.substr(pos + 1);
                    data[key] = value;
                }
            }
            
            // 填充游戏配置
            gameConfig.quickStart = (data["quickStart"] == "1") ? true : false;
            gameConfig.hasBuiltinAdmin = (data["hasBuiltinAdmin"] == "1") ? true : false;
            gameConfig.adminPassword = data["adminPassword"];
        }
        configFile.close();
    }
}

// 保存游戏配置
void saveGameConfig() {
    // 读取现有配置文件内容
    map<string, vector<string>> sections;
    string currentSection;
    
    ifstream inFile("config.ini");
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            if (line[0] == '[') {
                currentSection = line;
            } else if (!currentSection.empty() && !line.empty()) {
                sections[currentSection].push_back(line);
            }
        }
        inFile.close();
    }
    
    // 写入配置文件
    ofstream configFile("config.ini");
    if (configFile.is_open()) {
        // 写入游戏配置区块
        configFile << "[GameConfig]" << endl;
        configFile << "quickStart=" << (gameConfig.quickStart ? "1" : "0") << endl;
        configFile << "hasBuiltinAdmin=" << (gameConfig.hasBuiltinAdmin ? "1" : "0") << endl;
        configFile << "adminPassword=" << gameConfig.adminPassword << endl;
        configFile << endl;
        
        // 写入其他现有区块
        for (auto& section : sections) {
            if (section.first != "[GameConfig]") {
                configFile << section.first << endl;
                for (auto& line : section.second) {
                    configFile << line << endl;
                }
                configFile << endl;
            }
        }
        
        configFile.close();
    }
}

bool login() {
    // 如果启用快速启动，直接以Administrator用户登录
    if (gameConfig.quickStart && gameConfig.hasBuiltinAdmin) {
        currentUser.username = "Administrator";
        currentUser.password = "";
        currentUser.registerDate = getCurrentDate();
        currentUser.lastLoginDate = getCurrentDate();
        currentUser.health = 100;
        currentUser.maxHealth = 100;
        currentUser.fatigue = 100;
        currentUser.maxFatigue = 100;
        currentUser.hunger = 100;
        currentUser.thirst = 100;
        currentUser.warmth = 100;
        currentUser.fear = 0;
        currentUser.oxygen = 100;
        currentUser.money = 10000;
        currentUser.experience = 1000;
        currentUser.food = 50;
        currentUser.water = 50;
        currentUser.bandage = 20;
        currentUser.trapWire = 20;
        currentUser.mine = 10;
        currentUser.woodPickaxe = 1;
        currentUser.stonePickaxe = 1;
        currentUser.ironPickaxe = 1;
        currentUser.diamondPickaxe = 1;
        currentUser.woodPickaxeDurability = 50;
        currentUser.stonePickaxeDurability = 100;
        currentUser.ironPickaxeDurability = 200;
        currentUser.diamondPickaxeDurability = 1000;
        currentUser.ladders = 10;
        currentUser.ladderPlaced = false;
        currentUser.currentLadderLength = 0;
        currentUser.currentLadderDurability = 0;
        currentUser.backpackSize = 32;
        currentUser.storageBox = 3;
        currentUser.metalDetector = 2;
        currentUser.batteries = 50;
        currentUser.keepInventory = true;
        generateOreDeposits();
        // 尝试加载保存的数据（如果存在）
        loadUserData("Administrator");
        // 清除密码以确保安全
        currentUser.password = "";
        return true;
    }
    
    string username, password;
    cout << "=== 用户登录 ===" << endl;
    cout << "1. 登录" << endl;
    cout << "2. 注册" << endl;
    cout << "3. 退出游戏" << endl;
    cout << "请选择: ";
    
    int choice;
    cin >> choice;
    
    if (choice == 3) exit(0);
    if (choice == 2) return registerUser();
    if (choice == 1) {
        cout << "请输入用户名: ";
        cin >> username;
        cout << "请输入密码: ";
        cin >> password;
        
        // 检查是否为内置管理员账号
        if (gameConfig.hasBuiltinAdmin && username == "Administrator" && password == gameConfig.adminPassword) {
            try {
                // 重置currentUser到默认状态，避免数据残留导致的崩溃
                UserData newAdminUser;
                newAdminUser.username = "Administrator";
                newAdminUser.password = "";
                newAdminUser.registerDate = getCurrentDate();
                newAdminUser.lastLoginDate = getCurrentDate();
                newAdminUser.health = 100;
                newAdminUser.maxHealth = 100;
                newAdminUser.fatigue = 100;
                newAdminUser.maxFatigue = 100;
                newAdminUser.hunger = 100;
                newAdminUser.thirst = 100;
                newAdminUser.warmth = 100;
                newAdminUser.fear = 0;
                newAdminUser.oxygen = 100;
                newAdminUser.money = 10000;
                newAdminUser.experience = 1000;
                newAdminUser.food = 50;
                newAdminUser.water = 50;
                newAdminUser.bandage = 20;
                newAdminUser.trapWire = 20;
                newAdminUser.mine = 10;
                newAdminUser.woodPickaxe = 1;
                newAdminUser.stonePickaxe = 1;
                newAdminUser.ironPickaxe = 1;
                newAdminUser.diamondPickaxe = 1;
                newAdminUser.woodPickaxeDurability = 50;
                newAdminUser.stonePickaxeDurability = 100;
                newAdminUser.ironPickaxeDurability = 200;
                newAdminUser.diamondPickaxeDurability = 1000;
                newAdminUser.ladders = 10;
                newAdminUser.ladderPlaced = false;
                newAdminUser.currentLadderLength = 0;
                newAdminUser.currentLadderDurability = 0;
                newAdminUser.backpackSize = 32;
                newAdminUser.storageBox = 3;
                newAdminUser.metalDetector = 2;
                newAdminUser.batteries = 50;
                newAdminUser.keepInventory = true;
                
                // 不使用loadUserData，直接使用新的默认用户数据
                // 这样可以避免数据文件格式不匹配导致的闪退
                currentUser = newAdminUser;
                
                // 检查是否存在Administrator_fixed.dat文件（更安全的备份）
                ifstream fixedFile("Administrator_fixed.dat");
                if (fixedFile.is_open()) {
                    fixedFile.close();
                    cout << "检测到修复的数据文件，是否加载？(Y/N): ";
                    char choice;
                    cin >> choice;
                    if (choice == 'Y' || choice == 'y') {
                        if (loadUserData("Administrator_fixed")) {
                            cout << "修复的数据已加载！" << endl;
                            currentUser.password = "";
                        }
                    }
                }
                
                generateOreDeposits();
                cout << "管理员登录成功！" << endl;
                Sleep(1000);
                return true;
            } catch (...) {
                // 捕获所有异常，确保不会闪退
                cout << "管理员登录时发生错误，但已安全处理！" << endl;
                // 使用默认的管理员数据
                UserData safeAdminUser;
                safeAdminUser.username = "Administrator";
                safeAdminUser.password = "";
                safeAdminUser.registerDate = getCurrentDate();
                safeAdminUser.lastLoginDate = getCurrentDate();
                safeAdminUser.health = 100;
                safeAdminUser.maxHealth = 100;
                safeAdminUser.fatigue = 100;
                safeAdminUser.maxFatigue = 100;
                safeAdminUser.hunger = 100;
                safeAdminUser.thirst = 100;
                safeAdminUser.warmth = 100;
                safeAdminUser.fear = 0;
                safeAdminUser.oxygen = 100;
                safeAdminUser.money = 10000;
                safeAdminUser.experience = 1000;
                safeAdminUser.food = 50;
                safeAdminUser.water = 50;
                safeAdminUser.bandage = 20;
                safeAdminUser.backpackSize = 32;
                currentUser = safeAdminUser;
                Sleep(2000);
                return true;
            }
        }
        
        if (loadUserData(username)) {
            // 对于普通用户，密码可以为空（如果是从配置文件加载的）
            if (currentUser.password == "" || currentUser.password == password) {
                cout << "登录中，请稍后";
                for (int i = 0; i < 3; i++) {
                    cout << ".";
                    Sleep(500);
                }
                cout << "\n登录成功！" << endl;
                Sleep(1000);
                return true;
            } else {
                cout << "密码错误！" << endl;
                Sleep(1000);
                return false;
            }
        } else {
            cout << "用户不存在！" << endl;
            Sleep(1000);
            return false;
        }
    }
    return false;
}

/* ==========================  主函数  ========================== */
int main() {
    srand(static_cast<unsigned int>(time(0)));
    
    // 从配置文件加载游戏设置
    loadGameConfig();
    
    while (true) {
        // 如果启用快速启动，跳过加载动画和按Enter开始
        if (gameConfig.quickStart) {
            system("cls");
            centerText(Statistics);
            cout << "                                           FR Interactive Software" << endl;
            cout << "                                                Statistics v6.0" << endl;
            cout << "快速启动中..." << endl;
            Sleep(500);
        } else {
            // 显示欢迎界面
            system("cls");
            centerText(Statistics);
            cout << "                                           FR Interactive Software" << endl;
            cout << "                                                Statistics v6.0" << endl;
            cout << "请按Enter开始..." << endl;
            cin.ignore();
            cin.get();
            
            showProgressBar();
        }
        
        system("cls");
        centerText(Statistics);
        
        if (!login()) continue;
        
        bool logout = false;
        while (!logout) {
            system("cls");
            centerText(Statistics);
            cout << "欢迎, " << currentUser.username << "!" << endl;
            cout << "时间: " << (currentUser.isNight ? "夜晚" : "白天") << " | 第" << currentUser.dayCount << "天" << endl;
            cout << "金钱: " << currentUser.money << " 元 | 经验: " << currentUser.experience << " | 等级: " << (currentUser.experience / 100 + 1) << endl;
            cout << "背包: " << getTotalItemCount() << "/" << currentUser.backpackSize;
            if (currentUser.storageBox > 0) cout << " | 储物箱: " << currentUser.storageBox;
            cout << endl;
            showStatusBar();
            cout << "今日: 抽奖" << currentUser.lotteryCount << "/5 | 总挖矿: " << currentUser.totalMining << " 次" << endl;
            if (currentUser.depth > 0) {
                cout << "当前位置: 深度 " << currentUser.depth << "米";
                if (currentUser.ladderPlaced) {
                    cout << " | 梯子: " << currentUser.currentLadderLength << "米";
                    if (currentUser.currentLadderLength < currentUser.depth) 
                        cout << " (不够!)";
                } else {
                    cout << " | 警告: 无梯子!";
                }
                cout << endl;
            }
            cout << "=========================================" << endl;
            cout << "A.商店   B.信息   C.抽奖   D.设置   E.挖矿   F.退出游戏" << endl;
            cout << "请选择: " << endl;
            
            char choice;
            cin >> choice;
            
            if (choice == 'A' || choice == 'a') shop();
            else if (choice == 'B' || choice == 'b') showUserInfo();
            else if (choice == 'C' || choice == 'c') lottery();
            else if (choice == 'D' || choice == 'd') {
                settings();
                if (currentUser.username == "") logout = true;
            }
            else if (choice == 'E' || choice == 'e') mining();
            else if (choice == 'F' || choice == 'f') {
                cout << "感谢游玩！再见！" << endl;
                return 0;
            }
            else {
                cout << "无效选择！" << endl;
                Sleep(1000);
            }
            
            if (!checkSurvival()) break;
        }
    }
    return 0;
}