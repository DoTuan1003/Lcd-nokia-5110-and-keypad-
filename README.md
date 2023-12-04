How to run?
1. Build
`+cd driver/
     run: sudo make
`+cd user/
     run: sudo make
`+cd lib/
     run:sudo make
2. run
	+ insmod button_ctr.ko
	+ insmod lcd_driver.ko
- run ./user/mysnake	<speed>
- ex :
	+ ./user/mysnake 30
