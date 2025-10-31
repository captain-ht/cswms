# CSWMS - 工业自动化监控系统

CSWMS（Control System Warehouse Management System）是一个基于 Qt 框架开发的工业自动化监控和数据采集系统。该系统提供了完整的设备通信、实时数据监控、历史数据存储和可视化界面功能。

## 功能特性

### 🎯 核心功能

- **实时数据监控**：实时采集和显示工业设备数据
- **多协议支持**：支持多种工业通信协议
- **历史数据存储**：SQLite 数据库存储历史数据
- **可视化界面**：友好的图形用户界面，支持自定义样式
- **报警管理**：支持邮件报警和事件记录
- **用户权限管理**：完善的用户认证和权限控制系统

### 🔌 支持的通信协议

- **Modbus RTU**：串口 Modbus RTU 协议
- **Modbus ASCII**：串口 Modbus ASCII 协议
- **TCP/IP Modbus**：以太网 Modbus TCP/IP 协议
- **Siemens S7-200**：西门子 S7-200 PLC 通信
- **Mitsubishi FX2N**：三菱 FX2N PLC 通信

### 📊 系统模块

- **设备管理**：设备配置、连接管理和状态监控
- **数据采集**：实时数据采集和处理
- **数据库管理**：SQLite 数据库配置和操作
- **网络配置**：TCP/IP 网络参数配置
- **用户管理**：用户账户和权限管理
- **报警系统**：报警事件记录和邮件通知
- **可视化组件**：自定义控件和图表展示

## 技术栈

- **开发框架**：Qt 5.x / Qt 6.x
- **编程语言**：C++11
- **数据库**：SQLite
- **日志系统**：Log4Qt
- **图表库**：QCustomPlot
- **串口通信**：QExtSerialPort
- **属性浏览器**：Qt Property Browser

## 系统要求

### 编译要求

- Qt 5.6+ 或 Qt 6.0+（推荐 Qt 5.15+）
- C++11 或更高版本的编译器
- CMake 或 qmake 构建工具

### 运行时要求

- Windows 7+ / Linux / macOS
- SQLite 数据库支持
- 网络连接（用于 Modbus TCP/IP 和邮件功能）

## 编译指南

### 使用 qmake 编译

```bash
# 克隆仓库
git clone https://github.com/captain-ht/cswms.git
cd cswms

# 生成 Makefile
qmake cswms.pro

# 编译（Windows 使用 nmake，Linux/macOS 使用 make）
# Windows (MinGW)
mingw32-make

# Linux/macOS
make

# 编译发布版本
qmake CONFIG+=release cswms.pro
make
```

### 配置选项

在 `cswms/cswms.pro` 中可以配置以下编译选项：

- `cswmstool`：编译设备模拟调试工具
- `scada`：启用组态设计模块
- `ssl`：启用邮件 SSL 支持

## 目录结构

```
cswms/
├── 3rd_*              # 第三方库（Log4Qt, QCustomPlot, QExtSerialPort 等）
├── core_*             # 核心功能模块
│   ├── core_control   # 控件组件
│   ├── core_customplot# 图表组件
│   ├── core_db        # 数据库模块
│   ├── core_port      # 通信端口模块
│   ├── core_tag       # 标签管理
│   ├── core_vendor    # 供应商插件
│   └── ...
├── cswms/             # 主应用程序
│   ├── class/         # 业务逻辑类
│   ├── ui/            # 用户界面
│   └── file/          # 资源文件
├── Vendors/           # 设备驱动插件
│   ├── ModbusRTU      # Modbus RTU 驱动
│   ├── ModbusASCII    # Modbus ASCII 驱动
│   ├── TCPIPModbus    # Modbus TCP/IP 驱动
│   ├── S7_200         # 西门子 S7-200 驱动
│   └── FX2N           # 三菱 FX2N 驱动
└── README.md          # 项目说明文档
```

## 使用说明

### 首次运行

1. 启动应用程序
2. 系统会提示登录（如果未开启自动登录）
3. 进入系统后，首先配置：
   - **设备配置**：添加和配置工业设备
   - **网络设置**：配置 TCP/IP 参数
   - **数据库设置**：配置数据库连接
   - **用户管理**：创建用户账户

### 设备配置

1. 进入"配置" → "设备配置"
2. 添加新设备，选择通信协议类型
3. 配置设备地址、端口等参数
4. 保存配置并启动设备连接

### 数据监控

- 实时查看设备数据
- 查看历史数据趋势图
- 设置报警阈值和事件

## 注意事项

⚠️ **安全提示**：

- 在生产环境中，请勿在代码中硬编码密码和敏感信息
- 邮箱配置应从配置文件或环境变量中读取
- 建议定期更新数据库密码和用户权限

## 许可证

本项目采用 [MIT License](LICENSE) 开源协议。

## 贡献

欢迎提交 Issue 和 Pull Request 来帮助改进项目。

## 作者

- **captain-ht** - [GitHub](https://github.com/captain-ht)

## 致谢

- [Qt Framework](https://www.qt.io/)
- [QCustomPlot](http://www.qcustomplot.com/)
- [Log4Qt](https://sourceforge.net/projects/log4qt/)
- 以及所有其他使用的开源库

## 更新日志

### Version 2021.09.25

- 初始版本发布
- 支持多种工业通信协议
- 完整的监控和数据采集功能

---

如有问题或建议，请通过 GitHub Issues 联系。
