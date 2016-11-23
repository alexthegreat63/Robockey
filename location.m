% L1 = [492	330	10	473	368	8	567	367	10	513	402	5];
% l1 = [0 492 330; 0 473 368; 0 567 367; 0 513 402];
load('B.mat')
l1test = [0 0 0 0 0 0 0 0];
missedpoint = 0;
for k = 2:1:length(rawStarData)
    l1testprev =l1test;
    missedpointprev = missedpoint;
missedpoint = 0;
 l1test = rawStarData(k, :);
    for(n=1:4)
    if(rawStarData(k,n)==1023||rawStarData(k,n+4)==1023)
        if(missedpoint==1)
            missedpoint=missedpointprev;
            l1test= l1testprev;
            n=5;
        elseif(missedpoint==0)
            missedpoint = 1;
        if(n<4&& ~(rawStarData(k,4)==1023||rawStarData(k,8)==1023))
            l1test(n)=l1test(4);
            l1test(n+4)=l1test(8);
        end
        end
    end
    end
    
    
Pmax1 = 1;
Pmax2 = 2;
Dmax = 0;
P2max1 = 1;
P2max2 = 3;
D2max = 0;
if(missedpoint==0)
for(i=1:3)
    for(j=i+1:4)
        d = (l1test(i)-l1test(j))^2+(l1test(i+4)-l1test(j+4))^2;
        if(d>Dmax)
            P2max1 = Pmax1;
            P2max2 = Pmax2;
            D2max = Dmax;
            Pmax1 = i;
            Pmax2 = j;
            Dmax = d;
        else if(d>D2max)
            P2max1 = i;
            P2max2 = j;
            D2max = d;  
        end
        end
    end
end
if(Pmax1==P2max1 || Pmax1== P2max2)
    c=Pmax1;
    a=Pmax2;
else
    c=Pmax2;
    a=Pmax1;
end
%rawStarData(k+1,c)=1023; %to test a missing delete point a and go back one
%rawStarData(k+1,c+4)=1023;
%k=k-1 %^
theta = atan2(l1test(a+4)-l1test(c+4),l1test(a)-l1test(c));%-pi/2; 
Positionx = (l1test(a)+l1test(c))/2-512;
PositionY = (l1test(a+4)+l1test(c+4))/2-384;
end



Pmax1 = 1;
Pmax2 = 2;
Dmax = 0;
P2max1 = 1;
P2max2 = 3;
D2max = 0;

if(missedpoint==1)
for(i=1:2)
    for(j=i:3)
        d = (l1test(i)-l1test(j))^2+(l1test(i+4)-l1test(j+4))^2;
        if(d>Dmax)
            P2max1 = Pmax1;
            P2max2 = Pmax2;
            D2max = Dmax;
            Pmax1 = i;
            Pmax2 = j;
            Dmax = d;
        else if(d>D2max)
            P2max1 = i;
            P2max2 = j;
            D2max = d;  
        end
        end
    end
end

if(Pmax1==P2max1 || Pmax1== P2max2)
    c=Pmax1;
    a=Pmax2;
else
    c=Pmax2;
    a=Pmax1;
end
%Dmax/D2max
%Dmax
theta = atan2(l1test(a+4)-l1test(c+4),l1test(a)-l1test(c));%+pi/2; 
if(Dmax>8700)% (Dmax/D2max<1.5&&|| (Dmax/D2max>1.5&&Dmax) %d or b missing
Positionx = (l1test(a)+l1test(c))/2-512;
PositionY = (l1test(a+4)+l1test(c+4))/2-384;

elseif(Dmax/D2max<1.7) %a missing
 theta=-theta+.4648-pi;
% % Positionx = l1test(c)-512;
% % PositionY = l1test(c+4)-384;
 Positionx =14.5686/26*sqrt(Dmax)*cos(.9273+theta-pi)+l1test(c)-512;
  PositionY = 14.5686/26*sqrt(Dmax)*sin(.9273+theta-pi)+l1test(c+4)-384;%14.5686/26*
else %c missing
    theta=theta-pi/2+.27455;%+.27455;1.80167
    Positionx = -10.85/23*sqrt(Dmax)*cos(-theta+pi/2-.23087)+l1test(c)-512;
    PositionY = 10.85/23*sqrt(Dmax)*sin(-theta+pi/2-.23087)+l1test(c+4)-384;
end

%plot([Positionx Positionx-20*cos(theta)],[PositionY PositionY-20*sin(theta)])
hold on
end


%[max,maxi]=sort(l1)
Positionxbot = PositionY*cos(theta)-Positionx*sin(theta);
PositionYbot = -Positionx*cos(theta)-PositionY*sin(theta);
thetabot= -theta+pi/2 ;
quiver(Positionxbot, PositionYbot,20*cos(thetabot),20*sin(thetabot));
%plot([Positionxbot Positionxbot+20*cos(theta)],[PositionYbot PositionYbot+20*sin(theta)])
%plot([k*10,20*cos(theta)+k*10],[k*10,20*sin(theta)+k*10]);%Positionx],[PositionY],'r.')
hold on
pause(.01)
if (missedpoint==2)
    plot(0,k*2,'*')
    hold on
end
% for(m=1:4)
%     plot(rawStarData(k,m)-512,rawStarData(k,m+4)-384,'.')
%     hold on
% end
end
axis equal