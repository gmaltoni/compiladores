var x, y : integer;
    p : longint;
    z: char;
    w: string;
    t: boolean;
begin
    z := 'w';
    w := 'z';
    t := true;
    x := 2;
    y := -2;
    p := 234567345354345; 
    while y < -1 do
      begin
      x := y;
      x := x + 45;
      y := y + 1
      end;
    if x > 3 then
      begin
        x := 2 * y + 4;
        y :=  7;
      end
    else
      begin
        y := 5;
        x := 4;
      end;
    repeat
      begin
      y := 7;
      x := 6;
      end;
    until x < 5;
end.
