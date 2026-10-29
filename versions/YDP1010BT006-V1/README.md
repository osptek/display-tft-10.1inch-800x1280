<p align="left"><img alt="OSPTEK" src="../../images/logo.png" width="200" /></p>

<h1 align="center">OSPTEK 10.1″ TFT 显示屏 800×1280（YDP1010BT006-V1）</h1>

<p align="center"><b>TFT 模组 · MIPI · JD9366</b></p>

<p align="center"><a href="./README_EN.md">English</a> | 简体中文 · <a href="../../README.md">规格族索引</a></p>

<p align="center">
  <img alt="Interface: MIPI" src="https://img.shields.io/badge/Interface-MIPI-27AE60?style=flat-square" />
  <img alt="Driver: JD9366" src="https://img.shields.io/badge/Driver-JD9366-E7352C?style=flat-square" />
</p>

<p align="center"><img alt="OSPTEK 10.1 寸 800×1280 TFT 模组宣传图" src="./images/product.png" width="640" /></p>

## 目录

- [规格参数](#规格参数)
- [相关资料](#相关资料)
- [仓库结构](#仓库结构)
- [购买链接](#购买链接)
- [技术支持](#技术支持)

---

## 规格参数

| 项目 | 规格 |
| ---- | ---- |
| 尺寸 | 10.1 英寸 |
| 类型 | TFT |
| 分辨率 | 800×1280 |
| 外形尺寸 | 148.00 x 235.00 mm |
| 有效显示区域 | 135.36 x 216.58 mm |
| 接口 | MIPI |
| 驱动 IC | JD9366 |
| 触摸驱动 | — |

> 完整外形尺寸、FPC 定义、供电与时序以产品规格书 / 驱动手册为准。

## 相关资料

| 分类 | 文件 |
| ---- | ---- |
| 规格文档 | [`JD9366TC_User_Guide_Preliminary_V0.01_20220916.pdf`](./datasheets/JD9366TC_User_Guide_Preliminary_V0.01_20220916.pdf) |
| 规格文档 | [`YDP101BT006-V1.pdf`](./datasheets/YDP101BT006-V1.pdf) |
| CAD 图纸 | [`YDP101BT006-V1.dwg`](./cad/YDP101BT006-V1.dwg) |
| 转接板资料 | [`adapter-board-schematic.jpg`](./adapter/adapter-board-schematic.jpg) |
| 转接板资料 | [`PCB-10.1-P4-adapter-V1.1.pdf`](./adapter/PCB-10.1-P4-adapter-V1.1.pdf) |

## 仓库结构

```text
display-tft-10.1inch-800x1280/ # 仓库根（导航见 ../../README.md，示例工程见根目录 examples/）
└── versions/
    └── YDP1010BT006-V1/        # 本料号完整资料
        ├── README.md
        ├── README_EN.md
        ├── osptek.yml
        ├── images/
        ├── datasheets/       # 产品规格书 + 驱动/触摸 IC 数据手册
        ├── cad/              # 外形/盖板 CAD
        └── adapter/          # 转接板资料 + 转接板图片
```

## 购买链接

见[仓库首页](../../README.md#购买链接)。

## 技术支持

见[仓库首页](../../README.md#技术支持)。

---

<p align="center"><sub>© 2026 OSPTEK 鱼鹰光电 · 本仓库资料采用 CC BY 4.0 许可</sub></p>
