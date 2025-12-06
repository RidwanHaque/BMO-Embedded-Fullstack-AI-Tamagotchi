-- Database initialization script for BMO Chatbot
-- This script runs when the PostgreSQL container starts for the first time

-- Create the database (already created by POSTGRES_DB environment variable)
-- The database 'bmo_chatbot' is created automatically

-- Create any additional users or permissions if needed
-- (The main user 'bmo_user' is already created by POSTGRES_USER)

-- Create extensions if needed
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Create any additional tables or data that might be needed
-- (The main tables are created by Prisma migrations)

-- Insert any initial data
-- For example, you could add a welcome message or system configuration

-- Log the initialization
DO $$
BEGIN
    RAISE NOTICE 'BMO Chatbot database initialized successfully!';
END $$;
