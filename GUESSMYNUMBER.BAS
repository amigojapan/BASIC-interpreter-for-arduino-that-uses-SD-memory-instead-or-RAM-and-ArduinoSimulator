1 REM Guess my number
2 PRINT "1 to 100..."
3 REM RND is not implemented yet
4 REM 410 N = INT( RND(1) * 100 ) + 1
5 N=55
6 PRINT "Your guess? "
7 INPUT G
8 IF G=N THEN 11
9 IF G<N THEN 13
10 IF G>N THEN 15
11 PRINT "Got it!"
12 END
13 PRINT "Too low"
14 GOTO 6
15 PRINT "Too high"
16 GOTO 6
