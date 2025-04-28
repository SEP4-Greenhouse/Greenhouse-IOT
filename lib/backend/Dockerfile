# Use Node.js official image
FROM node:18

# Set working directory
WORKDIR /app

# Copy files
COPY package*.json ./
RUN npm install
COPY . .

# Start the server
CMD ["node", "server.js"]
