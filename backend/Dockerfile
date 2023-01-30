FROM python:3.10-alpine

# Set working directory
WORKDIR /app

# Install dependencies
COPY requirements.txt .

RUN python -m venv .env && \
    source .env/bin/activate && \
    pip install --upgrade pip && \
    pip install --no-cache-dir -r requirements.txt

# Set environment variables
ENV PYTHONDONTWRITEBYTECODE 1
ENV PYTHONUNBUFFERED 1  

# Copy source code
COPY . .

# Expose port
EXPOSE 8000

# Run the application
CMD [ "./.env/bin/gunicorn","--bind", "0.0.0.0:8000" ,"config.wsgi:application" ]