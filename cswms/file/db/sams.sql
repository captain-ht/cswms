PRAGMA foreign_keys = off;
BEGIN TRANSACTION;

-- 表：UserInfo
DROP TABLE IF EXISTS UserInfo;
CREATE TABLE UserInfo (UserName VARCHAR (20) NOT NULL, UserPwd VARCHAR (20) NOT NULL, UserType VARCHAR (10) NOT NULL, Permission1 VARCHAR (20), Permission2 VARCHAR (20), Permission3 VARCHAR (20), Permission4 VARCHAR (20), Permission5 VARCHAR (20), Permission6 VARCHAR (20), Permission7 VARCHAR (20), UserMark VARCHAR (30), PRIMARY KEY (UserName));
INSERT INTO UserInfo (UserName, UserPwd, UserType, Permission1, Permission2, Permission3, Permission4, Permission5, Permission6, Permission7, UserMark) VALUES ('admin', 'admin', '管理员', '启用', '启用', '启用', '启用', '启用', '启用', '启用', '');
INSERT INTO UserInfo (UserName, UserPwd, UserType, Permission1, Permission2, Permission3, Permission4, Permission5, Permission6, Permission7, UserMark) VALUES ('feiyang', '123456', '操作员', '启用', '启用', '禁用', '禁用', '禁用', '禁用', '禁用', '');

-- 表：AlarmLink
DROP TABLE IF EXISTS AlarmLink;
CREATE TABLE AlarmLink (PositionID VARCHAR (10) NOT NULL, ComName VARCHAR (5) NOT NULL, BaudRate INTEGER (6) NOT NULL, ModelAddr INTEGER (3) NOT NULL, LinkAddr VARCHAR (10) NOT NULL, LinkEnable VARCHAR (2) NOT NULL);

-- 表：AlarmLog
DROP TABLE IF EXISTS AlarmLog;
CREATE TABLE AlarmLog (LogID INTEGER PRIMARY KEY AUTOINCREMENT, PositionID VARCHAR (10), DeviceName VARCHAR (20), NodeName VARCHAR (20), NodeValue VARCHAR (5), NodeSign VARCHAR (5), Content VARCHAR (20), StartTime VARCHAR (19), EndTime VARCHAR (19), ConfirmUser VARCHAR (10), ConfirmTime VARCHAR (19), ConfirmContent VARCHAR (50));

-- 表：DeviceInfo
DROP TABLE IF EXISTS DeviceInfo;
CREATE TABLE DeviceInfo (DeviceID INTEGER PRIMARY KEY, PortName VARCHAR (20) NOT NULL, DeviceName VARCHAR (20) NOT NULL, DeviceAddr INTEGER (3) NOT NULL, DeviceType VARCHAR (10) NOT NULL, NodeNumber INTEGER (3) NOT NULL);

-- 表：NodeData
DROP TABLE IF EXISTS NodeData;
CREATE TABLE NodeData (PositionID VARCHAR (10) NOT NULL, NodeValue VARCHAR (5) NOT NULL, NodeStatus INTEGER (1) NOT NULL, SaveTime VARCHAR (19) NOT NULL);
INSERT INTO NodeData (PositionID, NodeValue, NodeStatus, SaveTime) VALUES ('AT-10001', '55', '1', '2019-06-23 12:25:26');
INSERT INTO NodeData (PositionID, NodeValue, NodeStatus, SaveTime) VALUES ('AT-10002', '58', '0', '2019-06-23 12:25:26');
INSERT INTO NodeData (PositionID, NodeValue, NodeStatus, SaveTime) VALUES ('AT-10004', '88', '2', '2019-06-23 12:25:26');
INSERT INTO NodeData (PositionID, NodeValue, NodeStatus, SaveTime) VALUES ('AT-10003', '63', '3', '2019-06-23 12:25:26');
INSERT INTO NodeData (PositionID, NodeValue, NodeStatus, SaveTime) VALUES ('AT-10005', '67', '4', '2019-06-23 12:25:26');

-- 表：NodeInfo
DROP TABLE IF EXISTS NodeInfo;
CREATE TABLE NodeInfo (NodeID INTEGER (3), PositionID VARCHAR (10) PRIMARY KEY, DeviceName VARCHAR (20) NOT NULL, NodeName VARCHAR (20) NOT NULL, NodeAddr INTEGER (3) NOT NULL, NodeType VARCHAR (10) NOT NULL, NodeClass VARCHAR (5) NOT NULL, NodeSign VARCHAR (5) NOT NULL, NodeUpper VARCHAR (5) NOT NULL, NodeLimit VARCHAR (5) NOT NULL, NodeMax VARCHAR (7) NOT NULL, NodeMin VARCHAR (5) NOT NULL, NodeRange VARCHAR (5) NOT NULL, NodeEnable VARCHAR (2) NOT NULL, NodeSound VARCHAR (10) NOT NULL, NodeImage VARCHAR (10) NOT NULL, SaveInterval INTEGER (3) NOT NULL, DotCount INTEGER (1) NOT NULL, AlarmDelay INTEGER (3) NOT NULL, AlarmType VARCHAR (2), NodeX INTEGER (4) NOT NULL, NodeY INTEGER (4) NOT NULL);

-- 表：NodeLog
DROP TABLE IF EXISTS NodeLog;
CREATE TABLE NodeLog (LogID INTEGER PRIMARY KEY AUTOINCREMENT, PositionID VARCHAR (10), DeviceName VARCHAR (20), NodeName VARCHAR (20), NodeValue VARCHAR (5), NodeSign VARCHAR (5), SaveTime VARCHAR (19));

-- 表：PortInfo
DROP TABLE IF EXISTS PortInfo;
CREATE TABLE PortInfo (PortID INTEGER PRIMARY KEY, PortName VARCHAR (30) NOT NULL, PortType VARCHAR (15) NOT NULL, ComName VARCHAR (4), BaudRate INTEGER (6), TcpIP VARCHAR (16), TcpPort INTEGER (6), ReadInterval INTEGER (4) NOT NULL, ReadTimeout INTEGER (4) NOT NULL, ReadMaxtime INTEGER (4) NOT NULL);

-- 表：TypeInfo
DROP TABLE IF EXISTS TypeInfo;
CREATE TABLE TypeInfo (TypeID INTEGER PRIMARY KEY, DeviceType VARCHAR (20), NodeNumber INTEGER (3), NodeType VARCHAR (20), NodeClass VARCHAR (10), NodeSign VARCHAR (10));
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (1, 'FC-1003-1', 1, 'FE-1102', 'GAS', 'PPM');
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (2, 'FC-1003-8', 8, 'FE-2102', 'O2', '%LEL');
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (3, 'FC-1003-16', 16, 'FT-1104', 'CO', '%VOL');
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (4, 'FC-1201显示板', 256, 'FT-2104', 'H2S', 'umol/mol');
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (5, 'FC-1201采样板', 4, 'FT-2104PLUS', 'C6H6', 'mg/m3');
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (6, 'FC-1201输出板', 256, 'FT-2104泵吸式', 'NH3', 'mA');
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (7, 'FC-1201通信板', 256, 'FT-2104拓展型', 'SO2', 'mV');
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (8, 'FT-2104P', 1, 'FT-2104单点型', 'NO', '℃');
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (9, 'FT-2104P传感器组件', 64, 'EA-08', 'NO2', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (10, 'SAMS-4128', 8, 'PT-1700', 'CL2', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (11, 'SAMS-423C', 12, 'PE-1100', 'HCLHCN', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (12, 'PT-5000', 5, 'PT-4100', 'CLO2', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (13, NULL, NULL, NULL, 'COCL2', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (14, NULL, NULL, NULL, 'PH3', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (15, NULL, NULL, NULL, 'ETO', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (16, NULL, NULL, NULL, 'H2', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (17, NULL, NULL, NULL, 'O3', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (18, NULL, NULL, NULL, 'CH', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (19, NULL, NULL, NULL, 'CH4', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (20, NULL, NULL, NULL, 'HF', NULL);
INSERT INTO TypeInfo (TypeID, DeviceType, NodeNumber, NodeType, NodeClass, NodeSign) VALUES (21, NULL, NULL, NULL, 'OTHER', NULL);

-- 表：UserLog
DROP TABLE IF EXISTS UserLog;
CREATE TABLE UserLog (LogID INTEGER PRIMARY KEY AUTOINCREMENT, TriggerTime VARCHAR (19), UserName VARCHAR (10), UserType VARCHAR (6), TriggerType VARCHAR (10), TriggerContent VARCHAR (15));

-- 索引：AlarmLogIndex
DROP INDEX IF EXISTS AlarmLogIndex;
CREATE INDEX AlarmLogIndex ON AlarmLog (LogID, DeviceName, Content, StartTime, EndTime, NodeName);

-- 索引：NodeLogIndex
DROP INDEX IF EXISTS NodeLogIndex;
CREATE INDEX NodeLogIndex ON NodeLog (LogID, PositionID, DeviceName, SaveTime, NodeName);

-- 索引：UserLogIndex
DROP INDEX IF EXISTS UserLogIndex;
CREATE INDEX UserLogIndex ON UserLog (LogID, TriggerTime, UserName, TriggerType);

COMMIT TRANSACTION;
PRAGMA foreign_keys = on;
