
# TODO: make this a parameter
N= (ARGV[0].to_i != 0) ? ARGV[0].to_i : 75

# create directory
Dir.mkdir("tc#{N}") unless Dir.exist?("tc#{N}")

# create up file
File.open("tc#{N}/edge.facts",'w') do |f|
    (1..N).each do |i| 
        f.write("#{rand(N)}\t#{rand(N)}\n")
    end
end

