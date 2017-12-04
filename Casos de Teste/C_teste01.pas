var
abacaxi:integer;
banana:real;
begin
abacaxi:=3;
banana:=4.1;
if abacaxi<banana then 
  begin 
    abacaxi:=5; 
    banana:=2; 
  end;
banana:=5;
if abacaxi<banana then abacaxi:=2;
banana:=3;
end.
