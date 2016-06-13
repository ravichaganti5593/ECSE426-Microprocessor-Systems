t = linspace(0,2*pi,100);
y = sin(t);
f = 3*y + transpose(rand(100,1));
g = filter(1/14*ones(1,14),1,f);
plot(g)
