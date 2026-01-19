# Idle 眨眼动画位图设计文档

## 📐 画布规格
- 尺寸：128 x 64 像素
- 格式：单色位图（1bpp）
- 颜色：黑底蓝色（像素 0=蓝色发光，像素 1=黑色不发光）

---

## 🎨 三个表情系列设计

### 1️⃣ Normal 系列（正常表情）

基于参数：
- Height: 40px, Width: 40px
- Radius_Top: 8px, Radius_Bottom: 8px
- Slope: 0 (水平)

#### 1.1 EMOJI_IDLE_NORMAL_OPEN (睁眼)
```
描述：两个圆润的大眼睛，居中显示
左眼中心：(32, 32)
右眼中心：(96, 32)
眼睛尺寸：40x40 圆角矩形，四角圆角半径 8px
绘制方式：
  - 外轮廓：圆角矩形边框（2px粗）
  - 瞳孔：中心圆形，直径 16px，实心填充
```

#### 1.2 EMOJI_IDLE_NORMAL_HALF (半闭眼)
```
描述：眼睛高度压缩到 20px
左眼中心：(32, 32)
右眼中心：(96, 32)
眼睛尺寸：40x20 (宽度不变，高度减半)
上眼睑：从上方遮盖 10px
下眼睑：保持不变
瞳孔：椭圆形 16x8
```

#### 1.3 EMOJI_IDLE_NORMAL_CLOSED (闭眼)
```
描述：完全闭合，显示为两条弧线
左眼位置：(12, 32) 到 (52, 32)
右眼位置：(76, 32) 到 (116, 32)
绘制方式：弧线，高度 4px，微微向下弯曲（半径 60px 的弧）
```

---

### 2️⃣ Sleepy 系列（困倦表情）

基于参数：
- Height: 14px, Width: 40px
- Slope_Top: -0.5, Slope_Bottom: -0.5
- Radius_Top: 3px, Radius_Bottom: 3px
- OffsetY: -2px

#### 2.1 EMOJI_IDLE_SLEEPY_OPEN (困倦睁眼)
```
描述：窄眼睛，上下眼睑均向外侧下垂
左眼中心：(32, 30) [OffsetY=-2]
右眼中心：(96, 30)
眼睛尺寸：40x14 (很窄)
上眼睑斜率：左高右低，slope=-0.5 (左边比右边高 20px*0.5=10px)
下眼睑斜率：同上
瞳孔：椭圆 12x6，位置偏下
外观：看起来眼皮很沉，快要睡着
```

#### 2.2 EMOJI_IDLE_SLEEPY_HALF (困倦半闭眼)
```
描述：高度进一步压缩到 8px
眼睛尺寸：40x8
几乎只剩一条缝隙
瞳孔：椭圆 10x4
```

#### 2.3 EMOJI_IDLE_SLEEPY_CLOSED (困倦闭眼)
```
描述：倾斜的闭眼线
左眼：从 (12, 25) 到 (52, 35) [斜线]
右眼：从 (76, 25) 到 (116, 35) [斜线]
绘制：2px 粗的斜线，向右下倾斜
```

---

### 3️⃣ Bored 系列（无聊表情）

基于 "无聊" 外观，半睁眼漫不经心

参数（自定义）：
- Height: 18px, Width: 40px
- Slope_Top: 0, Slope_Bottom: 0
- 眼睑位置：靠上

#### 3.1 EMOJI_IDLE_BORED_OPEN (无聊睁眼)
```
描述：半睁的椭圆眼睛，瞳孔偏上看
左眼中心：(32, 32)
右眼中心：(96, 32)
眼睛尺寸：40x18 (椭圆)
瞳孔：圆形直径 10px，位置在眼睛上半部 (32, 26) 和 (96, 26)
外观：眼睛半睁，眼神向上，显得漫不经心
```

#### 3.2 EMOJI_IDLE_BORED_HALF (无聊半闭眼)
```
描述：高度压缩到 10px
眼睛尺寸：40x10
瞳孔：椭圆 8x5，位置靠上
```

#### 3.3 EMOJI_IDLE_BORED_CLOSED (无聊闭眼)
```
描述：水平闭眼线，略微平直（不弯曲）
左眼：从 (12, 32) 到 (52, 32)
右眼：从 (76, 32) 到 (116, 32)
绘制：2px 粗直线
```

---

## 🔧 生成位图的方法

### 方法 1：使用 image2cpp 网站

1. 访问：https://javl.github.io/image2cpp/
2. 准备 128x64 的黑白 PNG 图片（按上述参数用绘图软件绘制）
3. 上传图片
4. 设置：
   - Canvas size: 128x64
   - Brightness: Black
   - Background color: Black (0x000000)
   - Invert image colors: ✅ 勾选（因为我们的像素逻辑是反向的）
   - Code output format: Arduino code, Horizontal - 1 bit per pixel
5. 生成代码，复制数组数据

### 方法 2：编程生成（推荐）

我可以直接用代码为你生成这些位图的数据数组，基于上述参数绘制几何图形。

---

## 📦 命名规范

```c
// emotion_bitmaps.h 中的枚举
typedef enum {
    // ... 现有的 8 个表情 ...

    // Idle Normal 系列
    EMOJI_IDLE_NORMAL_OPEN,
    EMOJI_IDLE_NORMAL_HALF,
    EMOJI_IDLE_NORMAL_CLOSED,

    // Idle Sleepy 系列
    EMOJI_IDLE_SLEEPY_OPEN,
    EMOJI_IDLE_SLEEPY_HALF,
    EMOJI_IDLE_SLEEPY_CLOSED,

    // Idle Bored 系列
    EMOJI_IDLE_BORED_OPEN,
    EMOJI_IDLE_BORED_HALF,
    EMOJI_IDLE_BORED_CLOSED,

    EMOTION_COUNT
} Emotion;

// emotion_bitmaps.c 中的数据
const EmotionBitmapInfo emoji_idle_normal_open = { ... };
const EmotionBitmapInfo emoji_idle_normal_half = { ... };
// ... 依此类推
```

---

## 🎯 下一步

你选择哪种方式生成位图数据？

**选项 A**：我用绘图软件（如 Paint.NET, GIMP）手工绘制 9 个 PNG，然后用 image2cpp 转换

**选项 B**：让我直接编程生成这 9 个位图的 C 数组数据（基于参数绘制圆形、椭圆、线条）

**选项 C**：先生成一个测试位图，看看效果再批量生成

---

**推荐选项 B**，我可以立即为你生成所有位图数据，无需手工绘制。
