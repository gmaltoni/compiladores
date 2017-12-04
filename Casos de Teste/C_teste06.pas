var a,b:integer;
    c,d:real;
    e,f:double;
    x,y:boolean;
begin
    a := 2;
    a := b;

    c := a;
    c := 1;
    c := 3.2;
    c := d;

    e := a;
    e := 1;
    e := 3.2;
    e := d;
    e := f;

    x := true;
    x := false;
    x := y;


    if x then true;
    if a > 3 then true;
    if 4.2 <> c then true;
    if x and y then true;

    a := 2 * 4 - b / a;
    x := true and false;
    x or false;
    e := a * c / e + (2 * 3);
    e := 3 * 4 + (3.4 + a) - c;

   while x or y and (y and true) do
   begin
        c := a*b + 2 * 4.3242;
   end;

   while a > e do
   begin
        c := a*b + 2 * 4.3242;
   end;

   repeat 
	a := 2 * 4 - b / a;
   	x := true and false;
   	x or false;
  	e := a * c / e + (2 * 3);
  	e := 3 * 4 + (3.4 + a) - c;
	a := a + 1;
   until
	20 < a;
end.
