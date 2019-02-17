OPEN "pressf10.raw" FOR BINARY AS #1
OPEN "pressf10.h" FOR OUTPUT AS #2
PRINT #2, "#define PRESSF10_WIDTH        94"
PRINT #2, "#define PRESSF10_HEIGHT       5"
PRINT #2, ""
PRINT #2, "unsigned char pressf10_graphics[PRESSF10_WIDTH][PRESSF10_HEIGHT] = {";
y = 0
FOR y = 0 TO 4
  FOR x = 0 TO 93
    a$ = " ": GET #1, , a$
    c = ASC(a$)

    'PSET (x, y), c
    PRINT #2, LTRIM$(RTRIM$(STR$(c)));
    IF x <> 93 THEN PRINT #2, ", ";
  NEXT
  PRINT #2, " \"
NEXT
 PRINT #2, "};"
CLOSE

