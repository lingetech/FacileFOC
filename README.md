# FacileFOC
## 项目介绍
一种适合新手的无刷电机驱动板及其控制程序。  
驱动板集成了：一路无刷电机控制器、一路串口。主控采用esp32。  
基于DengFOC开发了FacileFOC，逻辑更清晰。已实现的功能：开环速度控制、闭环位置控制、闭环速度控制。 
使用Arduino、C++开发esp32。  
## 已知缺陷
1. 仅适配19mm孔距的2804无刷电机。
2. cp2102的保护电路缺失，cp2102易损坏。
## 复刻指南
1. 从['/硬件/无刷电机控制器v2.8'](/硬件/无刷电机控制器v2.8)中可获取：
   1. 驱动板的工程文件“无刷电机控制器v2.8.eprj2”
      .eprj2文件需使用嘉立创EDA(专业版)3.x版本打开。
   3. 驱动板的原理图“SCH_编码器板_2026-01-30.pdf”和“SCH_驱动板_2026-01-30”
2. 从['/code'](/code)中获取控制程序。
   1. 参考[arduino-esp32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)
      1. 安装Arduine IDE
      2. 在Arduine IDE开发板管理器安装esp32支持。本项目采用3.2.0版本开发。
         2.x版本一定无法正常使用，因为3.x版本引入的新API“ledcAttach”合并并取代了2.x版本的API“ledcSetup”和“ledcAttachPin”。
         详见：[migration_guides/2.x_to_3.0](https://docs.espressif.com/projects/arduino-esp32/en/latest/migration_guides/2.x_to_3.0.html)。
   2. 使用Arduino IDE打开任一项目文件夹中的.ino文件。
   3. 调整“实例化对象”部分的参数以适配自己的硬件。
   4. 开发板选择“ESP32 Dev Module”，验证并上传程序至驱动板。
## 参考资料
1. 驱动板：  
   [鲲FOC无刷电机控制器（已适配灯哥开源FOC程序）](https://oshwhub.com/expert/xin-shuang-lu-simplefoc-wu-shua-dian-ji-qu-dong-qi-yi-shi-pei-deng-ge-foc-cheng-xu)  
   [AS5600无刷电机磁编码器板(兼容 2804/4010无刷电机)](https://oshwhub.com/expert/2804-dian-ji-ci-bian-ma-qi-ban)
2. 控制程序：
   [DengFOC](https://github.com/ToanTech/DengFOC_Lib)


