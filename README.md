电梯控制
----

#### 电梯状态  
0：静止状态  
1：上行状态  
2：下行状态 

电梯最大速度：2m/s  
电梯最大加速度：0.8m/s  
电梯由静止到最大速度用时：2.5s  
电梯由静止到最大速度位移：2.5m  

#### 电梯运行逻辑：  
动作1：电梯前方的顺电梯方向/无方向的满足速度要求的指令沿电梯方向搜索指令。  
动作2：所有楼层的逆电梯方向/无方向的满足速度要求的（执行至此时电梯速度一定为0）指令沿电梯方向的反方向搜索指令。  
动作3：电梯后方的顺电梯方向/无方向的满足速度要求的指令沿电梯方向搜索指令。  
步骤1：若电梯为静止状态，则将电梯状态改变为上行状态，执行步骤2。  
步骤2：按照动作1寻找目标指令，一旦找到则执行步骤5，若无满足要求指令则执行步骤3。  
步骤3：按照动作2寻找目标指令，一旦找到则执行步骤5，若无满足要求指令则执行步骤4。  
步骤4：按照动作3寻找指令，一旦找到则执行步骤5，若无满足要求指令则执行步骤6。  
步骤5：根据找到的指令和当前高度更新电梯方向。  
步骤6：没找到指令，电梯状态变为静止状态。  

#### 代码实现思路
- 命令用字符i+97表示，5层共计13个命令，为方便控制板处理数据，补全为15个命令。  
- 除静止状态的电梯外，指令不需要考虑先后。  
  故电梯板在有指令时，状态每100ms更新一次（与控制板通信）；无指令时状态实时更新。 


#### 难题及解决方案

难题：电梯状态复杂，难以通过串口发送  
解决：发送指令串到电梯板  

难题：sendorder挨个发送，如何保证全部接受  
解决：规范发送格式  

难题：由于100ms更新一次，并不能保证在匀减速的情况下，精准停在某一层，计算安全距离也不能实时运算  
解决：计算安全距离时打100ms提前量，在减速最后100ms直接将电梯速度降为零，并精准停靠  

难题：  
解决：  


#### 电梯性能  
- 命令计先后（误差在1ms左右）  
- 无漏接收（误差在1ms左右）   

#### 一些检验电梯逻辑的极端情况
1. 在中间层的各个时间有下一层的指令发出（即时性）  
分析：加入速度系统  
2. 在电梯当前楼层之上某一层有向下的指令，电梯到达该层后，高楼层向上/下的指令和电梯内到向楼层的指令同时发出（其他层无待处理指令）（可预测性）  
分析：2中的情况取决于（有方向的）指令的方向是否起作用，若不起作用，则有可能出现将一名向下的客人拉到很高的位置再送下来的情况，应该在到达该楼层后按照指令方向（顺电梯方向指令优先）更新电梯方向，若指令无方向，则继承之前电梯方向。  
3. 在电梯当前楼层之上某一层有向下的指令，电梯到达该层后，在电梯内向低楼层指令发出之前，高楼层有上/下的指令发出（其他层无待处理指令）  
分析：若3电梯判断为向上而2判断为向下，仍然有可能出现将一名向下的客人拉到很高的位置再送下来的情况，应等待电梯关门之后再寻找目标指令。  
4. 静止状态的电梯，同时接到当前楼层向上和向下指令，应该先处理向上指令。   


开关门的加入  
- 相关逻辑判断  

1. 在电梯当前楼层之上某一层有向下的指令，电梯到达该层后，高楼层向上/下的指令和电梯内到向楼层的指令同时发出（其他层无待处理指令）  

    上述情况取决于（有方向的）指令的方向是否起作用，若不起作用，则有可能出现将一名向下的客人拉到很高的位置再送下来的情况，应该在到达该楼层后按照指令方向（顺电梯方向指令优先）更新电梯方向。  

2. 在电梯当前楼层之上某一层有向下的指令，电梯到达该层后，在电梯内向低楼层指令发出之前，高楼层有上/下的指令发出（其他层无待处理指令）  

    若2电梯判断为向上而1判断为向下，仍然有可能出现将一名向下的客人拉到很高的位置再送下来的情况，应等待电梯关门之后再寻找目标指令（仍接收）。  

- 开关门指令  

1. 新加入了开门和关门两种指令，电梯在接到开门指令后不再运动，同时不再寻找目标指令（仍接收）。在接到关门指令后等待电梯门关上后恢复正常行动。  

2. 停靠时控制板自动向电梯发出开门指令，并触发Arduino中断计时器，实现接收命令的同时（若计时不被打断）在一定时间后发出关门指令。  

3. 电梯内部有开门和关门按钮，除向电梯发出开门和关门指令外，开门可触发中断计时器开始计时，关门可打断正在计时的计时器。  

4. 开关门按钮只在电梯停靠时有效。  

自动返回一层  

- 相关逻辑判断  
1. 到达一层后电梯不需要开门  
2. 回到一层过程中若接到指令，停止返回一层，执行目标指令，若目标指令在电梯后方，就近停靠后（不开门）再执行新指令。  
3. 回到一层过程中电梯状态为下行，即搜索指令按下行规则搜索。  
- 归位指令  
1. 新加入归位指令，电梯接到归位指令返回一层，电梯状态改为下行。  
2. 归位指令达成后判断为不开门。  
3. 电梯一旦进入静止状态（指令串为空），即触发中断计时器，若不被打断则一定时间后向指令串中添加归位指令。 
4. 电梯一旦脱离静止状态（有指令加入），则中断计时，并清除指令串中的归位指令；同时根据目标指令所在位置决定电梯运行。  


#### Things to improve
- [ ] Check timer here to alert users that they are holding the door open to long
- [ ] 更智能的返回一层策略
