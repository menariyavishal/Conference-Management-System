# API Design Guide

REST API design patterns and specifications for the Conference Management System.

## Overview

The API follows REST conventions with:
- **Protocol**: HTTP/HTTPS with TLS encryption (production)
- **Port**: 8080 (development), 443 (production)
- **Base URL**: `http://localhost:8080/api/v1` (development)
- **Data Format**: JSON
- **Authentication**: JWT tokens in Authorization header
- **Versioning**: URL-based (/api/v1, /api/v2, etc.)

## Response Format

### Success Response

```json
{
  "success": true,
  "data": {
    "userId": 1,
    "email": "user@example.com",
    "role": "AUTHOR"
  },
  "message": "User created successfully"
}
```

### Paginated Response

```json
{
  "success": true,
  "data": [
    { "id": 1, "title": "Paper 1" },
    { "id": 2, "title": "Paper 2" }
  ],
  "pagination": {
    "page": 1,
    "pageSize": 10,
    "totalCount": 25,
    "totalPages": 3
  },
  "message": "Success"
}
```

### Error Response

```json
{
  "success": false,
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Email format is invalid"
  }
}
```

### Error Codes

| Code | HTTP Status | Meaning |
|------|-------------|---------|
| VALIDATION_ERROR | 400 | Input validation failed |
| UNAUTHORIZED | 401 | Missing or invalid authentication |
| ACCESS_FORBIDDEN | 403 | Authenticated but not authorized |
| NOT_FOUND | 404 | Resource does not exist |
| CONFLICT | 409 | Resource already exists or conflict |
| DATABASE_ERROR | 500 | Database operation failed |
| INTERNAL_ERROR | 500 | Unexpected server error |
| FILE_ERROR | 500 | File operation failed |
| EMAIL_ERROR | 500 | Email sending failed |

## Authentication

### Obtaining a Token

```http
POST /api/v1/auth/login
Content-Type: application/json

{
  "email": "user@example.com",
  "password": "SecurePass123!"
}
```

Response:
```json
{
  "success": true,
  "data": {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "refreshToken": "refresh_token_here",
    "expiresIn": 86400
  },
  "message": "Login successful"
}
```

### Using the Token

All protected endpoints require the Authorization header:

```http
GET /api/v1/users/profile
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

### Token Refresh

```http
POST /api/v1/auth/refresh
Content-Type: application/json

{
  "refreshToken": "refresh_token_here"
}
```

## Common Endpoint Patterns

### Create Resource

```http
POST /api/v1/conferences
Content-Type: application/json
Authorization: Bearer <token>

{
  "name": "Academic Conference 2024",
  "description": "Annual conference...",
  "startDate": "2024-06-01",
  "endDate": "2024-06-03",
  "location": "New York, NY"
}
```

Response:
```json
{
  "success": true,
  "data": {
    "conferenceId": 1,
    "name": "Academic Conference 2024",
    "createdAt": "2024-05-05T10:30:00.000Z"
  },
  "message": "Conference created successfully"
}
```

### List Resources

```http
GET /api/v1/conferences?page=1&pageSize=10&sortBy=startDate&order=asc
Authorization: Bearer <token>
```

Query Parameters:
- `page` - Page number (1-indexed)
- `pageSize` - Items per page (default 10, max 100)
- `sortBy` - Field to sort by
- `order` - asc or desc
- `filter` - Additional filters (e.g., ?filter[status]=ACTIVE)

### Retrieve Single Resource

```http
GET /api/v1/conferences/1
Authorization: Bearer <token>
```

Response:
```json
{
  "success": true,
  "data": {
    "conferenceId": 1,
    "name": "Academic Conference 2024",
    "startDate": "2024-06-01",
    "endDate": "2024-06-03",
    "submissionDeadline": "2024-05-15",
    "reviewDeadline": "2024-05-25",
    "finalDecisionDeadline": "2024-05-31",
    "location": "New York, NY",
    "isActive": true,
    "createdAt": "2024-05-05T10:30:00.000Z",
    "updatedAt": "2024-05-05T10:30:00.000Z"
  },
  "message": "Success"
}
```

### Update Resource

```http
PUT /api/v1/conferences/1
Content-Type: application/json
Authorization: Bearer <token>

{
  "name": "Updated Conference Name",
  "location": "Los Angeles, CA"
}
```

Response:
```json
{
  "success": true,
  "data": {
    "conferenceId": 1,
    "name": "Updated Conference Name",
    "updatedAt": "2024-05-05T11:00:00.000Z"
  },
  "message": "Conference updated successfully"
}
```

### Delete Resource

```http
DELETE /api/v1/conferences/1
Authorization: Bearer <token>
```

Response:
```json
{
  "success": true,
  "data": null,
  "message": "Conference deleted successfully"
}
```

## HTTP Status Codes

| Code | Usage |
|------|-------|
| 200 | GET, PUT, PATCH success |
| 201 | POST success (resource created) |
| 204 | DELETE success (no content) |
| 400 | Bad request (validation error) |
| 401 | Missing authentication |
| 403 | Invalid permissions |
| 404 | Resource not found |
| 409 | Conflict (duplicate, constraint violation) |
| 500 | Server error |
| 503 | Service unavailable |

## Rate Limiting

Protected endpoints implement rate limiting:

```
X-RateLimit-Limit: 1000
X-RateLimit-Remaining: 999
X-RateLimit-Reset: 1620000000
```

Limits:
- Public endpoints: 100 requests/hour
- Authenticated endpoints: 1000 requests/hour
- Login endpoint: 10 attempts/5 minutes

## Pagination Best Practices

### Request

```http
GET /api/v1/submissions?page=2&pageSize=25
```

### Response

```json
{
  "success": true,
  "data": [...],
  "pagination": {
    "page": 2,
    "pageSize": 25,
    "totalCount": 150,
    "totalPages": 6,
    "hasNextPage": true,
    "hasPreviousPage": true
  }
}
```

### Navigation Links (Optional)

```json
{
  "links": {
    "first": "/api/v1/submissions?page=1",
    "previous": "/api/v1/submissions?page=1",
    "current": "/api/v1/submissions?page=2",
    "next": "/api/v1/submissions?page=3",
    "last": "/api/v1/submissions?page=6"
  }
}
```

## Filtering Examples

### Single Filter

```http
GET /api/v1/submissions?status=ACCEPTED
```

### Multiple Filters

```http
GET /api/v1/submissions?status=ACCEPTED&conferenceId=1&year=2024
```

### Date Range Filter

```http
GET /api/v1/submissions?submittedAfter=2024-01-01&submittedBefore=2024-12-31
```

### Nested Filter

```http
GET /api/v1/submissions?author[role]=AUTHOR
```

## Bulk Operations

### Bulk Create

```http
POST /api/v1/users/bulk
Content-Type: application/json
Authorization: Bearer <token>

[
  {
    "email": "user1@example.com",
    "firstName": "John",
    "lastName": "Doe",
    "role": "AUTHOR"
  },
  {
    "email": "user2@example.com",
    "firstName": "Jane",
    "lastName": "Smith",
    "role": "REVIEWER"
  }
]
```

Response:
```json
{
  "success": true,
  "data": {
    "created": 2,
    "failed": 0,
    "results": [
      { "status": "success", "userId": 1 },
      { "status": "success", "userId": 2 }
    ]
  }
}
```

### Bulk Update

```http
PATCH /api/v1/submissions/bulk
Content-Type: application/json
Authorization: Bearer <token>

{
  "ids": [1, 2, 3],
  "updates": {
    "status": "ACCEPTED"
  }
}
```

## File Upload

### Single File

```http
POST /api/v1/submissions/1/files
Content-Type: multipart/form-data
Authorization: Bearer <token>

file: <binary data>
```

Response:
```json
{
  "success": true,
  "data": {
    "fileId": 1,
    "filename": "paper.pdf",
    "size": 2048576,
    "uploadedAt": "2024-05-05T10:30:00.000Z"
  }
}
```

### Multiple Files

```http
POST /api/v1/submissions/1/files/bulk
Content-Type: multipart/form-data
Authorization: Bearer <token>

files: <file1>, <file2>, <file3>
```

## Search API

### Full-Text Search

```http
GET /api/v1/submissions/search?q=machine%20learning&fields=title,abstract
```

Response:
```json
{
  "success": true,
  "data": [
    {
      "submissionId": 1,
      "title": "Machine Learning Applications",
      "highlights": {
        "title": "Machine Learning Applications",
        "abstract": "...we apply <em>machine learning</em> techniques..."
      }
    }
  ]
}
```

### Advanced Search

```http
GET /api/v1/submissions/search?q=machine&authors=john,jane&year=2024
```

## Versioning Strategy

### Backward Compatibility

- New query parameters are optional
- Old endpoints remain functional
- Deprecated endpoints return warnings

### Major Version Change

```http
GET /api/v2/conferences  # New version
GET /api/v1/conferences  # Still available
```

Deprecation header:
```
Deprecation: true
Sunset: Sun, 31 Dec 2024 23:59:59 GMT
```

## CORS Headers

All responses include CORS headers for browser requests:

```
Access-Control-Allow-Origin: http://localhost:3000
Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS
Access-Control-Allow-Headers: Content-Type, Authorization
Access-Control-Allow-Credentials: true
Access-Control-Max-Age: 3600
```

## Caching Headers

```http
GET /api/v1/conferences/1

Cache-Control: public, max-age=300
ETag: "abc123def456"
Last-Modified: Sun, 05 May 2024 10:30:00 GMT
```

### Conditional Requests

```http
GET /api/v1/conferences/1
If-None-Match: "abc123def456"
```

Response (if not modified):
```
HTTP/1.1 304 Not Modified
```

## WebHooks (Planned)

### Register Webhook

```http
POST /api/v1/webhooks
Content-Type: application/json
Authorization: Bearer <token>

{
  "url": "https://your-app.com/webhooks/conference",
  "events": ["submission.created", "review.submitted"],
  "secret": "webhook_secret_key"
}
```

### Webhook Payload

```json
{
  "id": "evt_123",
  "type": "submission.created",
  "timestamp": "2024-05-05T10:30:00.000Z",
  "data": {
    "submissionId": 1,
    "title": "...",
    "authorId": 5
  }
}
```

### Webhook Signature

```
X-Webhook-Signature: sha256=hmac_hash
```

Verify using secret:
```
sha256(body + secret) == header_signature
```

## API Client Examples

### cURL

```bash
# Create submission
curl -X POST http://localhost:8080/api/v1/submissions \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "title": "My Paper",
    "abstract": "..."
  }'
```

### JavaScript/Fetch

```javascript
const response = await fetch('http://localhost:8080/api/v1/submissions', {
  method: 'POST',
  headers: {
    'Authorization': `Bearer ${token}`,
    'Content-Type': 'application/json'
  },
  body: JSON.stringify({
    title: 'My Paper',
    abstract: '...'
  })
});

const result = await response.json();
```

### Python

```python
import requests

headers = {
    'Authorization': f'Bearer {token}',
    'Content-Type': 'application/json'
}

response = requests.post(
    'http://localhost:8080/api/v1/submissions',
    headers=headers,
    json={
        'title': 'My Paper',
        'abstract': '...'
    }
)

data = response.json()
```

## OpenAPI/Swagger Specification

OpenAPI specification available at:
```
GET /api/v1/docs/openapi.json
GET /api/v1/docs/swagger-ui.html  # Interactive documentation
```

## Testing

### Unit Test Example

```cpp
TEST(ConferenceAPI, CreateConference) {
    json request = {
        {"name", "Test Conference"},
        {"startDate", "2024-06-01"}
    };
    
    auto response = api.createConference(request);
    
    EXPECT_EQ(response["success"], true);
    EXPECT_GT(response["data"]["conferenceId"], 0);
}
```

### Integration Test Example

```cpp
TEST(ConferenceAPI, FullWorkflow) {
    // Create conference
    // Submit paper
    // Assign reviewers
    // Submit reviews
    // Make decisions
    // Verify results
}
```

## Performance Optimization

### Request Compression

All endpoints support gzip compression:

```http
GET /api/v1/submissions
Accept-Encoding: gzip, deflate
```

### Response Compression

```
Content-Encoding: gzip
Content-Length: 1024
```

### Async Operations

Long-running operations return 202 Accepted with polling endpoint:

```http
POST /api/v1/reports/generate
```

Response:
```json
{
  "success": true,
  "data": {
    "jobId": "job_123",
    "statusUrl": "/api/v1/reports/job_123/status"
  }
}
```

Polling:
```http
GET /api/v1/reports/job_123/status
```
