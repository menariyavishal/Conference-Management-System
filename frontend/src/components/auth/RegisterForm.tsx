import React, { useState } from 'react';
import { useAuth } from '../../context/AuthContext';
import authService from '../../services/auth.service';
import { useNavigate } from 'react-router-dom';
import { toast } from 'react-hot-toast';
import type { UserRole } from '../../types/api';

interface RegisterFormProps {
  onSwitch: () => void;
}

const INTEREST_OPTIONS = [
  { id: 'AI', label: '🤖 AI' },
  { id: 'Quantum', label: '⚛️ Quantum' },
  { id: 'Cybersecurity', label: '🔒 Cybersecurity' },
  { id: 'BioTech', label: '🧬 BioTech' },
  { id: 'HCI', label: '🖥️ HCI' },
];

const RegisterForm: React.FC<RegisterFormProps> = ({ onSwitch }) => {
  const [formData, setFormData] = useState({
    firstName: '',
    lastName: '',
    email: '',
    password: '',
    confirmPassword: '',
    affiliation: '',
    role: 'AUTHOR' as UserRole,
  });
  const [selectedInterests, setSelectedInterests] = useState<string[]>([]);
  const [isLoading, setIsLoading] = useState(false);
  
  const { login } = useAuth();
  const navigate = useNavigate();

  const toggleInterest = (id: string) => {
    setSelectedInterests(prev => 
      prev.includes(id) ? prev.filter(i => i !== id) : [...prev, id]
    );
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (formData.password !== formData.confirmPassword) {
      toast.error("Passwords do not match");
      return;
    }
    
    setIsLoading(true);
    try {
      // Transform frontend data to backend format
      const registrationData = {
        email: formData.email,
        password: formData.password,
        fullName: `${formData.firstName} ${formData.lastName}`,
        role: formData.role.toLowerCase(),
        affiliation: formData.affiliation,
        interests: selectedInterests
      };
      
      console.log("Sending registration data:", registrationData);
      const response = await authService.register(registrationData);
      console.log("Registration response:", response);
      
      if (response.success) {
        toast.success("Account created successfully!");
        // Auto-login after registration
        try {
          const loginRes = await authService.login({ email: formData.email, password: formData.password });
          console.log("Login response after registration:", loginRes);
          if (loginRes.success) {
            const userData = {
              id: loginRes.data.userId || loginRes.data.id,
              userId: loginRes.data.userId,
              email: loginRes.data.email,
              fullName: loginRes.data.fullName,
              role: loginRes.data.role?.toUpperCase(),
              isActive: true,
              createdAt: new Date().toISOString(),
              updatedAt: new Date().toISOString()
            };
            login(loginRes.data.token, userData as any);
            navigate('/dashboard');
          }
        } catch (loginError) {
          console.error("Auto-login failed:", loginError);
          toast("Account created! Please log in manually.");
          navigate('/login');
        }
      } else {
        // Handle error response - could be error.message or error.error.message
        const errorMsg = typeof response.error === 'object' 
          ? response.error?.message 
          : response.error || "Registration failed";
        toast.error(String(errorMsg));
      }
    } catch (error: any) {
      console.error("Registration error:", error);
      toast.error(error.message || error || "Registration failed");
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="p-8">
      <div className="text-center mb-6">
        <h2 className="text-3xl font-bold bg-gradient-to-r from-blue-400 to-purple-400 bg-clip-text text-transparent">
          Create Account
        </h2>
        <p className="text-slate-400 mt-1">Join the research community</p>
      </div>

      <form onSubmit={handleSubmit} className="space-y-4">
        <div className="grid grid-cols-2 gap-3">
          <div>
            <label className="block text-xs text-slate-400 mb-1">First Name</label>
            <input
              type="text"
              required
              className="w-full bg-white/5 border border-white/10 rounded-xl px-3 py-2 text-white text-sm focus:border-blue-500"
              value={formData.firstName}
              onChange={(e) => setFormData({ ...formData, firstName: e.target.value })}
            />
          </div>
          <div>
            <label className="block text-xs text-slate-400 mb-1">Last Name</label>
            <input
              type="text"
              required
              className="w-full bg-white/5 border border-white/10 rounded-xl px-3 py-2 text-white text-sm focus:border-blue-500"
              value={formData.lastName}
              onChange={(e) => setFormData({ ...formData, lastName: e.target.value })}
            />
          </div>
        </div>

        <div>
          <label className="block text-xs text-slate-400 mb-1">Email</label>
          <input
            type="email"
            required
            className="w-full bg-white/5 border border-white/10 rounded-xl px-3 py-2 text-white text-sm focus:border-blue-500"
            value={formData.email}
            onChange={(e) => setFormData({ ...formData, email: e.target.value })}
          />
        </div>

        <div className="grid grid-cols-2 gap-3">
          <div>
            <label className="block text-xs text-slate-400 mb-1">Password</label>
            <input
              type="password"
              required
              minLength={8}
              className="w-full bg-white/5 border border-white/10 rounded-xl px-3 py-2 text-white text-sm focus:border-blue-500"
              value={formData.password}
              onChange={(e) => setFormData({ ...formData, password: e.target.value })}
            />
          </div>
          <div>
            <label className="block text-xs text-slate-400 mb-1">Confirm</label>
            <input
              type="password"
              required
              className="w-full bg-white/5 border border-white/10 rounded-xl px-3 py-2 text-white text-sm focus:border-blue-500"
              value={formData.confirmPassword}
              onChange={(e) => setFormData({ ...formData, confirmPassword: e.target.value })}
            />
          </div>
        </div>

        <div>
          <label className="block text-xs text-slate-400 mb-1">Affiliation</label>
          <input
            type="text"
            required
            placeholder="MIT, Stanford, etc."
            className="w-full bg-white/5 border border-white/10 rounded-xl px-3 py-2 text-white text-sm focus:border-blue-500"
            value={formData.affiliation}
            onChange={(e) => setFormData({ ...formData, affiliation: e.target.value })}
          />
        </div>

        <div>
          <label className="block text-xs text-slate-400 mb-1">Primary Role</label>
          <select
            className="w-full bg-white/5 border border-white/10 rounded-xl px-3 py-2 text-white text-sm focus:border-blue-500 appearance-none"
            value={formData.role}
            onChange={(e) => setFormData({ ...formData, role: e.target.value as UserRole })}
          >
            <option value="AUTHOR" className="bg-[#1e293b]">Author (Submit papers)</option>
            <option value="REVIEWER" className="bg-[#1e293b]">Reviewer (Evaluate papers)</option>
            <option value="ORGANIZER" className="bg-[#1e293b]">Organizer (Manage events)</option>
          </select>
        </div>

        <div>
          <label className="block text-xs text-slate-400 mb-2">Interests (Optional)</label>
          <div className="flex flex-wrap gap-2">
            {INTEREST_OPTIONS.map(opt => (
              <span
                key={opt.id}
                onClick={() => toggleInterest(opt.id)}
                className={`px-3 py-1 rounded-full text-[10px] font-bold cursor-pointer transition-all duration-200 border ${
                  selectedInterests.includes(opt.id)
                    ? 'bg-blue-600 border-blue-500 text-white shadow-lg shadow-blue-500/20'
                    : 'bg-white/5 border-white/10 text-slate-400 hover:bg-white/10'
                }`}
              >
                {opt.label}
              </span>
            ))}
          </div>
        </div>

        <button
          type="submit"
          disabled={isLoading}
          className="btn-primary w-full py-3 text-sm font-bold mt-2"
        >
          {isLoading ? 'Creating account...' : 'Sign Up'}
        </button>
      </form>

      <div className="mt-5 text-center">
        <button
          type="button"
          onClick={onSwitch}
          className="text-blue-400 hover:underline text-xs"
        >
          ← Back to Login
        </button>
      </div>
    </div>
  );
};

export default RegisterForm;
