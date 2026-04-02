# UART21 Pin Toggle Demo

这个 demo 会把 `P1.08` 和 `P1.09` 配置成普通 GPIO 输出，并每隔 3 秒同时翻转一次电平。

对应关系：

- `P1.08` = XIAO `D6`
- `P1.09` = XIAO `D7`

说明：

- `uart21` 已在 overlay 中禁用，这两根脚不再作为串口使用。
- 工程不启用 shell、log、console，不会输出串口信息。
