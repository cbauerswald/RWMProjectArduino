s = serial('COM3', 'BaudRate', 115200);
fopen(s);
pause(2);
fprintf(s, 'dump;');
fprintf('Reading data');
done = 0;
data = [];
while done==0
    line = strtrim(fgetl(s));
    if length(line)==0
        done = 1;
        fprintf('\n');
    else
        temp = textscan(line, '%f', 'Delimiter', ',');
        data = [data; temp{1}'];
        if mod(size(data, 1), 1000)==0
            fprintf('.');
        end
    end
end
fclose(s);
clear done line temp s;
% byte_address = 1;

accel_data = zeros(round(length(data)*4/5),4);
for i = 1:length(data)
    if mod(i-1,5) ~= 0
       accel_data(floor((i-1)/5)+1,mod(i-1,5)) = data(i);  
    end
end
accel_data(:,4) = accel_data(:,4)./256*100; %convert to milliseconds from int
time_heading = 0;
for i = 2:length(accel_data)
    if accel_data(i-1,4)>accel_data(i,4)
        time_heading = time_heading + 0.1; % 0.1 seconds, or 100 milliseconds
    end    
    accel_data(i,4) = accel_data(i,4) + time_heading;
end
figure;
hold on;
plot(accel_data(:,4),accel_data(:,1),'r')
plot(accel_data(:,4),accel_data(:,2),'b')
plot(accel_data(:,4),accel_data(:,3),'g')

% accel_data = zeros(round(length(data)/14),7); %x,y,z, time
% for i = 1:length(data) %because of matlab indexing
%     if mod(i-1,2) == 0
%         %parse byte address
%         incoming_address = data(i); %parse number here
%         if incoming_address ~= byte_address
%             disp('Error, byte address out of order')
%         end
%         byte_address = byte_address+1;
%     end
%     if mod(i-1,14) == 1
%         accel_data(i-1,1) = data(i);
%     end
%     if mod(i-1,14) == 3
%         accel_data(i-1,2) = data(i);
%     end
%     if mod(i-1,14) == 5
%         accel_data(i-1,3) = data(i);
%     end
%     if mod(i-1,14) == 7
%         accel_data(i-1,4) = data(i);
%     end
%     if mod(i-1,14) == 9
%         accel_data(i-1,5) = data(i);
%     end
%     if mod(i-1,14) == 11
%         accel_data(i-1,6) = data(i);
%     end
%     %time byte
%     if mod(i-1,14) == 13
%         accel_data(i-1,7) = data(i);
%     end
% end
% %parse time data
% time_2msd = 0;
% for i = 2:length(accel_data)
%     if accel_data(i-1,7)>accel_data(i,7)
%         time_2msd = time_2msd + 0.1;
%     end
%     accel_data(i,7) = accel_data(i,7) + time_2msd;
% end
% 

