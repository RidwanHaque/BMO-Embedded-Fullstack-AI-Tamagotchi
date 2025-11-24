FROM node:18-alpine

WORKDIR /app

# Install OS dependencies required by Prisma
RUN apk add --no-cache openssl

# Copy package files
COPY package*.json ./

# Install dependencies
RUN npm install

# Copy source code
COPY . .

# Generate Prisma client inside the image
RUN npx prisma generate

# Expose port
EXPOSE 3001

# Start the application
CMD ["npm", "run", "dev"]
